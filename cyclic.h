#pragma once

template<class T> class Cyclic
{
	bool dummy;
	size_t size;
	size_t capacity;
	size_t power;
	size_t offset;
	T* data;

	Cyclic(bool d, size_t s, size_t c, size_t p, size_t o, T* dd) : dummy(d), size(s), capacity(c), power(p), offset(o), data(dd){}
public:
	Cyclic(): dummy(false), size(0), capacity(0), power(0), offset(0), data(0){}
	Cyclic(const Cyclic& other): data(0)
	{
		*this = other;
	}
	Cyclic(size_t power) : dummy(false), size(0), capacity((size_t)1 << power), power(power), offset(0), data(new T[capacity]()){}
	~Cyclic()
	{
		if(!dummy) delete[] data;
	}
	Cyclic& operator= (const Cyclic& other)
	{
		if (this != &other)
		{
			delete[] data;
			dummy = other.dummy;
			size = other.size;
			capacity = other.capacity;
			power = other.power;
			offset = other.offset;
			if (capacity)
			{
				data = new T[capacity]();
				for (size_t i = 0; i < capacity; ++i)
				{
					data[i] = other.data[i];
				}
			}
		}
	}
	T& operator[] (size_t index)
	{
		return data[(index + offset) & (capacity - 1)];
	}
	const T& operator[] (size_t index) const
	{
		return data[(index + offset) & (capacity - 1)];
	}
	Cyclic operator+ (size_t lhs) const
	{
		return Cyclic(true, size, capacity, power, (offset + lhs) & (capacity - 1), data);
	}
	void Initialize(size_t power)
	{
		if (!dummy)
		{
			delete[] data;
			dummy = false;
			size = 0;
			this->power = power;
			capacity = (size_t)1 << power;
			offset = 0;
			data = new T[capacity]();
		}
	}
	void push_back(const T& element)
	{
		if (size != capacity)
		{
			data[size++] = element;
		}
		else
		{
			data[offset] = element;
			offset += 1;
			if (offset == capacity) offset = 0;
		}
	}
	size_t GetSize() const
	{
		return size;
	}
};
