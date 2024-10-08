#pragma once
#include <src/raylib-cpp.hpp>
#include <Eigen/core>
#include "../spatialHash/types.h"
#include "../spatialHash/hash.h"
#include "../Components/EntitySpecific.h"
#include "../Components/Transforms.h"
#include "../globals.h"
#include "../EntityIds.h"
#include <unordered_map>
#include <set>
#include <optional>
#include <type_traits>


namespace spatial_hash {
	struct Tile {
		std::set<ECS::Entity> entitySet{};
	};

	class Grid {
	public:
		using Entity = ECS::Entity;
		using Rect = raylib::Rectangle;
		using vec = raylib::Vector2;
	private:

		LongIndexHashMap<Tile> tileMap;
		int tileSize;
		float tileSizeInv;
		uint32_t gridSize;
		int tileCount;
		uint32_t entityCount;
		

		/**
		* @brief turns xyz coordinates into Index coordinates
		* @param point - normal coordinates
		* @param tileLengthInv - constant for conversion
		*/
		template<typename IndexT>
		IndexT indexFromPoint(
			const Point& point,
			const float tileLengthInv
		) {
			return IndexT(
				(const int)std::floor(point.x() * tileLengthInv),
				(const int)std::floor(point.y() * tileLengthInv),
				(const int)std::floor(point.z() * tileLengthInv));
		}

		template<typename IndexT>
		IndexT pointFromIndex(
			const Point& point,
			const float tileLength
		) {
			return IndexT(
				std::floor(point.x() * tileLength),
				std::floor(point.y() * tileLength),
				std::floor(point.z() * tileLength));
		}

		
		bool checkTile(const LongIndex& point) {
			return !(tileMap.at(point).entitySet.empty());
		}

		/**
		* @param xyz - tile coordinates in grid
		*/
		bool checkInBounds(const int x, const int y, const int z) {
			return !(x < 0 || x > tileCount - 1|| y < 0 || y > tileCount - 1|| z < 0 || z > tileCount - 1);
		}
	public:
		/**
		* @param gridSize - must divisible by tileSize
		* @param tileSize - must be power of 2
		*/
		Grid(int tileSize, uint32_t gridSize) : tileSize(tileSize), tileSizeInv(1.0f / static_cast<float>(tileSize)), gridSize(gridSize), tileCount(gridSize / tileSize) {
			for (int x = 0; x < tileCount; x++) {
				for (int y = 0; y < tileCount; y++) {
					tileMap.insert({ LongIndex(x, y, 0), Tile() });
				}
			}
		}

		/**
		* @brief inserts enemy based on its size
		* @param entity - entities index
		* @param entityRect - size of entity
		*/
		void insert(Entity entity, Rect entityRect) {
			auto NW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(entityRect.x)), static_cast<const float>(static_cast<int>(entityRect.y)), 0), tileSizeInv);
			auto NE = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(entityRect.x + entityRect.width)), static_cast<const float>(static_cast<int>(entityRect.y)), 0), tileSizeInv);
			auto SW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(entityRect.x)), static_cast<const float>(static_cast<int>(entityRect.y + entityRect.height)), 0), tileSizeInv);
			
			for (auto x = NW.x(); x <= NE.x(); x++) {
				for (auto y = NW.y(); y <= SW.y(); y++) {
					if (checkInBounds(x, y, 0)) {
						tileMap.at(LongIndex(x, y, 0)).entitySet.insert(entity);
					}
				}
			}
		}
	
		void remove(Entity entity, vec entityPos) {
			
			auto pos = indexFromPoint<BlockIndex>(Point(static_cast<const float>(entityPos.x), static_cast<const float>(entityPos.y), 0), tileSizeInv);

			if (!checkInBounds(pos.x(), pos.y(), 0) || !tileMap.at(LongIndex(pos.x(), pos.y(), 0)).entitySet.contains(entity)) {
				return;
			}
			else {
				tileMap.at(LongIndex(pos.x(), pos.y(), 0)).entitySet.erase(entity);

				remove(entity, vec(entityPos.x + tileSize, entityPos.y));
				remove(entity, vec(entityPos.x - tileSize, entityPos.y));
				remove(entity, vec(entityPos.x, entityPos.y + tileSize));
				remove(entity, vec(entityPos.x, entityPos.y - tileSize));
			}
		}

		std::set<Entity> queryFromTile(BlockIndex tilePos) {
			return tileMap.at(LongIndex(tilePos.x(), tilePos.y(), tilePos.z())).entitySet;
		}

		std::set<Entity> query(Rect area) {
			std::set<Entity> entityQuery;
			auto NW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x)), static_cast<const float>(static_cast<int>(area.y)), 0), tileSizeInv);
			auto NE = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x + area.width)), static_cast<const float>(static_cast<int>(area.y)), 0), tileSizeInv);
			auto SW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x)), static_cast<const float>(static_cast<int>(area.y + area.height)), 0), tileSizeInv);
			
			
			for (auto x = NW.x(); x <= NE.x(); x++) {
				for (auto y = NW.y(); y <= SW.y(); y++) {
					if (checkInBounds(x, y, 0)) {
						for (const auto& e : tileMap.at(LongIndex(x, y, 0)).entitySet) {
							entityQuery.insert(e);
						}
					}
				}
			}

			return entityQuery;
		}

		std::set<Entity> query(vec pos, float radius) {
			std::set<Entity> entityQuery;
			raylib::Rectangle area{ pos - raylib::Vector2(radius, radius) * 0.5f, raylib::Vector2(radius, radius) };

			auto NW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x)), static_cast<const float>(static_cast<int>(area.y)), 0), tileSizeInv);
			auto NE = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x + area.width)), static_cast<const float>(static_cast<int>(area.y)), 0), tileSizeInv);
			auto SW = indexFromPoint<BlockIndex>(Point(static_cast<const float>(static_cast<int>(area.x)), static_cast<const float>(static_cast<int>(area.y + area.height)), 0), tileSizeInv);


			for (auto x = NW.x(); x <= NE.x(); x++) {
				for (auto y = NW.y(); y <= SW.y(); y++) {
					if (checkInBounds(x, y, 0)) {//TODO -- check x, y are in circle
						for (const auto& e : tileMap.at(LongIndex(x, y, 0)).entitySet) {
							entityQuery.insert(e);
						}
					}
				}
			}

			return entityQuery;
		}
		

		std::optional<Entity> queryNearestEntityById(vec pos, float radius, ID_ENTITY entityId) {
			std::list<Entity> entityListWithId;

			for (auto const& entity : query(pos, radius)) {
				auto const& entitySpecific = G::gCoordinator.GetComponent<EntitySpecific>(entity);
				if (entitySpecific.id == entityId) {
					entityListWithId.emplace_back(entity);
				}
			}

			if (entityListWithId.empty()) {
				return std::nullopt;
			}

			auto dist = std::numeric_limits<float>::max();
			Entity entityToGive{};

			for (auto const& entity : entityListWithId) {
				auto const& transform = G::gCoordinator.GetComponent<Transforms>(entity);
				auto dist_temp = Vector2Distance(pos, transform.position);

				if (dist_temp < dist) {
					dist = dist_temp;
					entityToGive = entity;
				}
			}

			return std::optional<Entity>{entityToGive};
		}
		
		void clearAllTiles() {
			for (auto& [m, t] : tileMap) {
				t.entitySet.clear();
			}
		}

		void drawPopulatedTiles() {
			for (auto& t : tileMap) {
				if (!t.second.entitySet.empty()) {
					DrawRectangleLines(t.first.x() * tileSize, t.first.y() * tileSize, tileSize, tileSize, RED);
					raylib::DrawText(std::to_string(t.second.entitySet.size()), t.first.x() * tileSize, t.first.y() * tileSize, 10, BLACK);
				}
			}
		}

		void drawGrid() {
			for (int i = 0; i < tileCount + 1; i++) {
				DrawLineV(Vector2{ (float)tileSize * i, 0 }, Vector2{ (float)tileSize* i, (float)gridSize }, LIGHTGRAY);
			}

			for (int i = 0; i < tileCount + 1; i++) {
				DrawLineV(Vector2{ 0, (float)tileSize* i }, Vector2{ (float)gridSize, (float)tileSize* i }, LIGHTGRAY);
			}

			for (int i = 0; i < tileCount; i++) {
				for (int j = 0; j < tileCount; j++) {
					DrawText(TextFormat("[%i,%i]", i, j), 10 + tileSize * i, 15 + tileSize * j, 10, LIGHTGRAY);
				}
			}
		}

		uint32_t getEntityCount() {
			return entityCount;
		}
	};

	inline Grid gGird(64, 5760);
}
