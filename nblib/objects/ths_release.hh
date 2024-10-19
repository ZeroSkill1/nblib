#ifndef nblib_objects_ths_release_hh
#define nblib_objects_ths_release_hh

#include <string>

namespace nb
{
	template <typename TString>
	struct NbThsRelease
	{
		u64 added;
		TString version;
		TString versiondesc;
		TString changelog;
		TString dlUrl;
		TString sourceUrl;
	};

	class ThsRelease : public NbThsRelease<std::string>
	{
	public:
		static constexpr const char *magic = "3HSR";

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbThsRelease<nb::BlobPtr>))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbThsRelease<nb::BlobPtr> *rhdr = (NbThsRelease<nb::BlobPtr> *)header;

			this->added = rhdr->added;

			char *strdata = (char *)blob;

			this->version = std::string(&strdata[rhdr->version]);
			this->versiondesc = std::string(&strdata[rhdr->versiondesc]);
			this->changelog = std::string(&strdata[rhdr->changelog]);
			this->dlUrl = std::string(&strdata[rhdr->dlUrl]);
			this->sourceUrl = std::string(&strdata[rhdr->sourceUrl]);

			return nb::StatusCode::SUCCESS;
		}
	};
};

#endif
