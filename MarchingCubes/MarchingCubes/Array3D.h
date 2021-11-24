#pragma once

template<typename T>
class Array3D {
private:
	T* _ptr;
	size_t _w, _h, _d;
public:
	Array3D(){}
	Array3D(size_t w, size_t h, size_t d);
	~Array3D();
	T*** getPtr() { return _ptr; }
	size_t getW() const { return _w; }
	size_t getH() const { return _h; }
	size_t getD() const { return _d; }
	T& At(size_t x, size_t y, size_t z) const;
	void free();
};

template<typename T>
inline Array3D<T>::Array3D(size_t w, size_t h, size_t d)
	:_w(w), _h(h), _d(d)
{
	_ptr = new T[w * h * d];
}

template<typename T>
inline Array3D<T>::~Array3D()
{

}

template<typename T>
inline T& Array3D<T>::At(size_t x, size_t y, size_t z) const
{
	return _ptr[x * _h * _d + y * _d + z];
}

template<typename T>
inline void Array3D<T>::free()
{
	delete[] _ptr;
}
