#include <utility>

template<typename T>
struct unique_ptr_deleter
{
	void operator()(T* resource) const
	{
		delete resource;
	}
};

// Takes in a default deleter. unique_ptr_deleters operator will be invoked by default.
template<typename T, typename deleter = unique_ptr_deleter<T>>
class unique_ptr
{
private:
	T * ptr_resource = nullptr;
	deleter operator_delete;
public:
	// Safely constructs resource. Operator new is called by the user. Once constructed the unique_ptr will own the resource.
	// std::move is used because it is used to indicate that an object may be moved from other resource.
	explicit unique_ptr(T* raw_resource) noexcept : ptr_resource(std::move(raw_resource)) {}
	unique_ptr(std::nullptr_t) : ptr_resource(nullptr) {}

	unique_ptr() noexcept : ptr_resource(nullptr) {}

	// destroys the resource when object goes out of scope. This will either call the default delete operator or a user-defined one.
	~unique_ptr() noexcept
	{
		operator_delete(ptr_resource);
	}
	// Disables the copy/ctor and copy assignment operator. We cannot have two copies exist or it'll bypass the RAII concept.
	unique_ptr(const unique_ptr<T>&) noexcept = delete;
	unique_ptr& operator = (const unique_ptr&) noexcept = delete;
	
public:
	// Allows move-semantics. While the unique_ptr cannot be copied it can be safely moved. 
	unique_ptr(unique_ptr&& move) noexcept
	{
		move.swap(*this);
	}
	// ptr = std::move(resource)
	unique_ptr& operator=(unique_ptr&& move) noexcept
	{
		move.swap(*this);
		return *this;
	}
	explicit operator bool() const noexcept
	{
		return this->ptr_resource;
	}
	// releases the ownership of the resource. The user is now responsible for memory clean-up.
	T* release() noexcept
	{
		return std::exchange(ptr_resource, nullptr);
	}
	// returns a pointer to the resource
	T* get() const noexcept
	{
		return ptr_resource;
	}
	// swaps the resources
	void swap(unique_ptr<T>& resource_ptr) noexcept
	{
		std::swap(ptr_resource, resource_ptr.ptr_resource);
	}
	// replaces the resource. the old one is destroyed and a new one will take it's place.
	void reset(T* resource_ptr) noexcept(false)
	{
		// ensure a invalid resource is not passed or program will be terminated
		if (resource_ptr == nullptr)
			throw std::invalid_argument("An invalid pointer was passed, resources will not be swapped");

		operator_delete(ptr_resource);

		ptr_resource = nullptr;

		std::swap(ptr_resource, resource_ptr);
	}
public:
	// overloaded operators
	T * operator->() const noexcept
	{
		return this->ptr_resource;
	}
	T& operator*() const noexcept
	{
		return *this->ptr_resource;
	}
	// May be used to check for nullptr
};
