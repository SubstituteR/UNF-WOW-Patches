#pragma once
template<typename T>
class extern_ptr
{
	T* address_ptr = 0;
	static inline auto from_module(LPCTSTR module, int offset) -> T*
	{
		return reinterpret_cast<T*>(reinterpret_cast<int>(GetModuleHandle(module)) + offset);
	}
public:
	explicit extern_ptr(T* address)
	{
		address_ptr = address;
	}

	extern_ptr(LPCTSTR module, int offset) : extern_ptr(from_module(module, offset)) {} /* module + offset */

	[[nodiscard]] auto get() -> T*
	{
		return address_ptr;
	}

	[[nodiscard]] auto operator*() -> T
	{
		return *address_ptr;
	}

	[[nodiscard]] auto operator->()  -> T
	{
		return *address_ptr;
	}

	auto reset(T* address)
	{
		address_ptr = address;
	}
	auto reset(LPCTSTR module, int offset) { reset(from_module(module, offset)); }

};