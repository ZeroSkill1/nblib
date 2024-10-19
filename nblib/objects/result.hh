#ifndef nblib_objects_result_hh
#define nblib_objects_result_hh

#include <string>

namespace nb
{
	template<typename TString>
	struct NbResult
	{
		u32 code;
		TString message;
	};

	class Result : public NbResult<std::string>
	{
	public:
		static constexpr const char *magic = "RSLT";

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbResult<nb::BlobPtr>))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbResult<nb::BlobPtr> *rhdr = (NbResult<nb::BlobPtr> *)header;

			this->code = rhdr->code;
			if (rhdr->message) this->message = std::string((char *)&blob[rhdr->message]);

			return nb::StatusCode::SUCCESS;
		}
	};
};

#endif
