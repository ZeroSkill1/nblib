#ifndef nblib_objects_top_title_hh
#define nblib_objects_top_title_hh

namespace nb
{
	template <typename TString>
	struct NbTopTitle
	{
		u32 id;
		u64 dlCount;
		TString name;
		TString alt;
	};

	class TopTitle : public NbTopTitle<std::string>
	{
	public:
		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbTopTitle<nb::BlobPtr>))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbTopTitle<nb::BlobPtr> *thdr = (NbTopTitle<nb::BlobPtr> *)header;

			this->id = thdr->id;
			this->dlCount = thdr->dlCount;

			char *strdata = (char *)blob;

			this->name = std::string(&strdata[thdr->name]);
			if (thdr->alt) this->alt = std::string(&strdata[thdr->alt]);

			return nb::StatusCode::SUCCESS;
		}
	};
};
#endif
