#ifndef inc_nb_hh
#define inc_nb_hh

#include <3ds/types.h>
#include <string.h>
#include <vector>

namespace nb
{
	typedef u32 BlobPtr;
	typedef BlobPtr ArrayPtr;

	static const char ArrayMagic[4] = { 'N', 'B', 'A', 'R' };

	struct ObjectHeader
	{
		char magic[4];
		u32  object_header_size;
		u32  header_size;
		u32  blob_size;

		inline u32 total_header_size() {
			return this->header_size + this->object_header_size;
		}

		inline u32 total_size() {
			return this->total_header_size() + this->blob_size;
		}
	};

	struct ArrayHeader
	{
		char magic[4];
		u32  array_header_size;
		u32  object_count;
		u32  element_header_size;
		u32  shared_blob_size;

		inline u32 total_elements_header_size() {
			return this->object_count * this->element_header_size;
		}

		inline u32 total_header_size() {
			return this->array_header_size + this->total_elements_header_size();
		}

		inline u32 total_size() {
			return this->total_header_size() + this->shared_blob_size;
		}
	};

	enum class StatusCode : int
	{
		SUCCESS              = 0,
		NO_INPUT_DATA        = 1,
		MAGIC_MISMATCH       = 2,
		INPUT_DATA_TOO_SHORT = 3,
		UNALIGNED            = 4,
		EMPTY_ARRAY          = 5,
	};

	template <typename T>
	static nb::StatusCode parse_full(T& out, u8 *data, size_t size, u32 *out_size = nullptr)
	{
		if (!data || !size)
			return nb::StatusCode::NO_INPUT_DATA;

		if ((u32) data & 3)
				return nb::StatusCode::UNALIGNED;

		if (memcmp(T::magic, data, 4) != 0)
			return nb::StatusCode::MAGIC_MISMATCH;

		if (size < sizeof(nb::ObjectHeader))
			return nb::StatusCode::INPUT_DATA_TOO_SHORT;

		nb::ObjectHeader *hdr = (nb::ObjectHeader *)data;

		if (hdr->object_header_size + hdr->header_size + hdr->blob_size > size)
			return nb::StatusCode::INPUT_DATA_TOO_SHORT;

		u8 *obj_hdr = &data[hdr->object_header_size];
		u8 *blob = &data[hdr->object_header_size + hdr->header_size];

		if (out_size) *out_size = hdr->object_header_size + hdr->header_size + hdr->blob_size;

		return out.deserialize(obj_hdr, hdr->header_size, blob, hdr->blob_size);
	}

	static nb::StatusCode validate_array_header(nb::ArrayHeader *&hdr, u8 *data, size_t size)
	{
		if (!data || !size)
			return nb::StatusCode::NO_INPUT_DATA;

		if ((u32) data & 3)
			return nb::StatusCode::UNALIGNED;

		if (memcmp(nb::ArrayMagic, data, 4) != 0)
			return nb::StatusCode::MAGIC_MISMATCH;

		if (size < sizeof(nb::ArrayHeader))
			return nb::StatusCode::INPUT_DATA_TOO_SHORT;

		nb::ArrayHeader *_hdr = static_cast<nb::ArrayHeader *>(data);

		if (hdr->element_header_size & 3)
			return nb::StatusCode::UNALIGNED;

		if (hdr->total_size() > size)
			return nb::StatusCode::INPUT_DATA_TOO_SHORT;

		if (!hdr->object_count)
			return nb::StatusCode::EMPTY_ARRAY;

		hdr = _hdr;

		return nb::StatusCode::SUCCESS;
	}

	/* if the entire object consists of an array */
	template <typename T>
	static nb::StatusCode parse_array(std::vector<T>& out, u8 *array, size_t bufsize, u32 *out_size = nullptr)
	{
		nb::ArrayHeader *hdr = nullptr;

		/* validate array header */
		nb::StatusCode c = nb::validate_array_header(hdr, array, bufsize);

		if (c != nb::StatusCode::SUCCESS) return c;

		out.reserve(hdr->object_count);

		u8 *array_blob = &array[this->total_elements_header_size()];

		nb::StatusCode c = nb::StatusCode::SUCCESS;

		for (u32 i = 0; i < hdr->object_count; i++)
		{
			u8 *obj_hdr = &array[hdr->array_header_size + i * hdr->element_header_size];

			T obj;

			if ((c = obj.deserialize(obj_hdr, hdr->element_header_size, array_blob, hdr->shared_blob_size)) != nb::StatusCode::SUCCESS)
				return c;

			out.push_back(obj);
		}

		if (out_size) *out_size = hdr->total_size();

		return nb::StatusCode::SUCCESS;
	}

	/* if the array is within another object */
	template <typename T>
	static nb::StatusCode parse_array_inline(std::vector<T>& out, ArrayPtr array_offset, u8 *blob, size_t blob_size, u32 *out_size = nullptr)
	{
		if (!array_offset) return nb::StatusCode::SUCCESS; /* empty array */
	
		if (array_offset >= blob_size) return nb::StatusCode::INPUT_DATA_TOO_SHORT; /* array offset is out of bounds */

		size_t remain_blob_size = blob_size - array_offset; /* space remaining in given blob starting from the array header to the end */

		u8 *raw_array = &blob[array_offset]; /* points to the array header in the blob */
		
		return nb::parse_array(out, raw_array, remain_blob_size, out_size); /* parse the array from the blob directly */
	}
}
#endif
