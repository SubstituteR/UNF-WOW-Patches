#pragma once
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <string>
#include <mutex>
class IPatch
{
public:
	virtual bool load() = 0;
	virtual bool unload() = 0;
};

class Patches
{
	static std::unordered_map<std::type_index, std::shared_ptr<IPatch>> appliedPatches; //std::type_index(typeid(int))
	static std::mutex patchesMutex;
public:
	template<typename T>
	static void Add()
	{
		const std::lock_guard<std::mutex> lock(patchesMutex);
		auto type = std::type_index(typeid(T));
		if (appliedPatches.find(type) != appliedPatches.end())
		{
			printf("This patch was already applied!\n");
			return;
		}
		auto _ = std::make_shared<T>();
		appliedPatches.insert({ type, _});
		printf("Added patch at %p\n", _);
		_->load();
	}

	template<typename T>
	static std::shared_ptr<IPatch> Get()
	{
		const std::lock_guard<std::mutex> lock(patchesMutex);
		auto type = std::type_index(typeid(T));
		if (appliedPatches.find(type) == appliedPatches.end())
			return nullptr;
		return appliedPatches[type];
	}

	template<typename T>
	static void Remove()
	{
		const std::lock_guard<std::mutex> lock(patchesMutex);
		auto type = std::type_index(typeid(T));
		if (appliedPatches.find(type) == appliedPatches.end())
			return;
		auto _ = appliedPatches[type];
		_->unload();
		_.reset();
		appliedPatches.erase(type);
	}

	static void Dump();
};