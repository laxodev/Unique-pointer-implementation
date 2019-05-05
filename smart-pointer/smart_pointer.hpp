#include <algorithm>

template<typename T>
class unique_ptr
{
private:
	T * ptr_resource = nullptr;
public:
	// Safely constructs resource. Operator new is called by the user. Once constructed the unique_ptr will own the resource.
	// std::move is used because it is used to indicate that an object may be moved from other resource.
	explicit unique_ptr(T* raw_resource) noexcept : ptr_resource(std::move(raw_resource)) {}
	unique_ptr(std::nullptr_t) : ptr_resource(nullptr) {}

	// destroys the resource when object goes out of scope
	~unique_ptr() noexcept
	{
		delete ptr_resource;
	}
	// Disables the copy/ctor and copy assignment operator. We cannot have two copies exist or it'll bypass the RAII concept.
	unique_ptr(const unique_ptr<T>&) noexcept = delete;
	unique_ptr& operator = (const unique_ptr&) noexcept = delete;
public:
	// releases the ownership of the resource. The user is now responsible for memory clean-up.
	T* release() noexcept
	{
		T* resource_ptr = this->ptr_resource;
		this->ptr_resource = nullptr;

		return resource_ptr;
	}
	// returns a pointer to the resource
	T* get() const noexcept
	{
		return ptr_resource;
	}
	// swaps the resources
	void swap(const unique_ptr& resource_ptr) noexcept(false)
	{
		std::swap(ptr_resource, resource_ptr.ptr_resource);
	}
	// replaces the resource. the old one is destroyed and a new one will take it's place.
	void reset(T* resource_ptr) noexcept(false)
	{
		// ensure a invalid resource is not passed or program will be terminated
		if (resource_ptr == nullptr)
			throw std::invalid_argument("An invalid pointer was passed, resources will not be swapped");

		delete ptr_resource;

		ptr_resource = nullptr;

		std::swap(ptr_resource, resource_ptr);
	}
public:
	// overloaded operators
	T* operator->() const noexcept
	{
		return this->ptr_resource;
	}
	T& operator*() const noexcept
	{
		return *this->ptr_resource;
	}
	// May be used to check for nullptr
};
