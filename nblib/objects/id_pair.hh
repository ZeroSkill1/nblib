#ifndef nblib_objects_id_pair_hh
#define nblib_objects_id_pair_hh

#include <string>

#include <nblib/nb.hh>

namespace nb
{
	struct NbIdPair { u64 tid; u32 id; };

	class idPair : public NbIdPair
	{
	public:
		static constexpr const char *magic = "PAIR";

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbIdPair))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbIdPair *phdr = (NbIdPair *)header;

			this->tid = phdr->tid;
			this->id = phdr->id;

			return nb::StatusCode::SUCCESS;
		}
	};
};

#endif
