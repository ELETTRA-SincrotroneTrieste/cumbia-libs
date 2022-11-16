#ifndef CUMATRIX_H
#define CUMATRIX_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h> // memcmp
#include <utility> // as_const

// use CuMatrixData instead of std::vector to easily implement operator []
// in CuMatrix
//
template<typename T>
class CuMatrix {
public:
    CuMatrix(const std::vector<T> &v, size_t dimx, size_t dimy)
        : nr(dimx), nc(dimy) {
		p = new T[nr * nc];
		std::copy(v.begin(), v.end(), p);
	}

    CuMatrix(T *v, size_t dimx, size_t dimy)
        : nr(dimx), nc(dimy) {
		p = new T[nr * nc];
		memcpy(p, v, nr * nc * sizeof(T));
	}

    /*! \brief Matrix constructor accepting pointer and sizes. const version
     */
    CuMatrix(const T *v, size_t dimx, size_t dimy)
        : nr(dimx), nc(dimy) {
        p = new T[nr * nc];
        memcpy(p, v, nr * nc * sizeof(T));
    }

	CuMatrix(const CuMatrix<T>& other) {
		m_from_other(other);
	}

	CuMatrix(CuMatrix<T> && other) {
		printf("CuMatrix move constr\n");
		/* no new p here! */
		p = other.p;
		other.p = nullptr; /* don't delete */
	}

    CuMatrix() : nr(0), nc(0) {
		p = new T[0];
	}

	~CuMatrix() {
		if(p)
			delete [] p;
	}

	T* operator [] (size_t idx) {
        return p + (idx * nc);
	}

	const T* operator [] (size_t idx) const {
        return p + (idx * nc);
	}

	bool operator ==(const CuMatrix<T> &other) const {
		return other.nr == nr && other.nc == nc && // compare std::vector to watch for non trivial T types
		        std::vector<T>(p, p + nr * nc) == std::vector<T>(other.p, other.p + nr * nc);
	}

	CuMatrix<T> &operator=(const CuMatrix<T> &other) {
		if(this != &other) {
			if(p) delete [] p;
			m_from_other(other);
		}
		return *this;
	}

	bool operator !=(const CuMatrix<T> &other) const {
		return !operator ==(other);
	}

	CuMatrix *clone() const {
		return new CuMatrix(*this);
	}

	std::vector<T> data() const {
		return std::vector<T>(p, p + nr * nc);
	}

	T* raw_data() const { return p; }

	size_t size() const { return nr * nc; }
	size_t nrows() const { return nr; }
	size_t ncols() const { return nc; }

	bool isValid() const { return nr > 0 && nc > 0; }

	static CuMatrix *from_string_matrix(const CuMatrix<std::string>& from) {
		return  new CuMatrix<std::string>(from.data(), from.nrows(), from.ncols());
	}

	/*!
	 * \brief repr return a string representation of the matrix
	 * \param maxlen truncate the output after *about* maxlen characters
	 * \return a string representation of the matrix in the form [a,b,c] [d,e,f] [...]
	 *
	 * \note maxlen is an *indication* of the maximum desired length: the last row in output is not truncated.
	 */
	std::string repr(size_t maxlen = -1) const {
		std::stringstream sst;
		sst << "(" << nr << "x" << nc << ") [ ";
		for(size_t r = 0; r < nr; r++) {
			sst << " [";
			for(size_t c = 0; c < nc; c++) {
				sst << this->operator[](r)[c];
				if(c < nc - 1) sst << std::string(",");
			}
			sst << "]";
			if(sst.str().length() > maxlen)
				break;
		}
		if(maxlen > 4 && sst.str().length() > maxlen - 4)
			sst << "...";
		sst << " ]";
		return sst.str();
	}


private:
	void m_from_other(const CuMatrix<T> &other) {
		p = new T[other.nr * other.nc];
		nr = other.nr;
		nc = other.nc;
		const std::vector<T>& v = other.data();
		std::copy(v.begin(), v.end(), p);
	}

	size_t nr, nc;
	T * p;
};

#endif
