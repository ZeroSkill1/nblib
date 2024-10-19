#ifndef nblib_nb_single_object_hh
#define nblib_nb_single_object_hh

#include <nblib/nb.hh>

namespace nb
{
	namespace single_object
	{
		struct ObjectHeader
		{
			NbMagic magic;
			u32  object_header_size;
			u32  header_size;
			u32  blob_size;

			inline u32 total_header_size() { return this->header_size + this->object_header_size; }
			inline u32 total_size() { return this->total_header_size() + this->blob_size; }
		};

		template <typename T>
		static nb::StatusCode parse(T& out, u8 *data, size_t size, u32 *out_size = nullptr)
		{
			if (!data || !size)
				return nb::StatusCode::NO_INPUT_DATA;

			if ((uintptr_t) data & 3)
				return nb::StatusCode::UNALIGNED;

			if (memcmp(T::magic, data, 4) != 0)
				return nb::StatusCode::MAGIC_MISMATCH;

			if (size < sizeof(ObjectHeader))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			ObjectHeader *hdr = reinterpret_cast<ObjectHeader *>(data);

			if (hdr->total_size() > size)
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			u8 *obj_hdr = &data[hdr->object_header_size];
			u8 *blob = &data[hdr->total_header_size()];

			nb::StatusCode sc = out.deserialize(obj_hdr, hdr->header_size, blob, hdr->blob_size);

			if (sc == nb::StatusCode::SUCCESS && out_size)
				*out_size = hdr->total_size();

			return sc;
		}
	};
};

#endif
