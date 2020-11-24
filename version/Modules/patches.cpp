#include <typeindex>
#include "./patches.h"

#define CLASS_PREFIX 6 /* The length of the class prefix in type id */

std::unordered_map<std::type_index, std::shared_ptr<IPatch>> Patches::appliedPatches = {};
std::mutex Patches::patchesMutex;

void Patches::Dump() {
	const std::lock_guard<std::mutex> lock(patchesMutex);
	for (auto&& patch : appliedPatches)
	{
		std::cout 
			<< "Patch \"" 
			<< patch.first.name() + CLASS_PREFIX
			<< "\"\t-------->\t" 
			<< patch.second 
			<< " (" 
			<< patch.second.use_count() 
			<< ")" << std::endl;
	}
}