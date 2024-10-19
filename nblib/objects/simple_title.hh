#ifndef nblib_objects_simple_title_hh
#define nblib_objects_simple_title_hh

namespace nb
{
	template <typename TString>
	struct NbSimpleTitle
	{
		u64 tid;
		u64 size;
		u64 flags;
		u64 dlCount;
		u32 id;
		TString name;
		TString alt; /* legacy! */
		TString prod;
		u16 version;
		u8 contentType;
		u8 cat;
		u8 subcat;
	};

	class SimpleTitle : public NbSimpleTitle<std::string>
	{
	public:
		std::vector<std::string> alt_names;

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbSimpleTitle<nb::BlobPtr>))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbSimpleTitle<nb::BlobPtr> *sthdr = (NbSimpleTitle<nb::BlobPtr> *)header;

			this->tid = sthdr->tid;
			this->size = sthdr->size;
			this->flags = sthdr->flags;
			this->dlCount = sthdr->dlCount;
			this->id = sthdr->id;
			this->version = sthdr->version;
			this->contentType = sthdr->contentType;
			this->cat = sthdr->cat;
			this->subcat = sthdr->subcat;

			char *strdata = (char *)blob;

			this->name = std::string(&strdata[sthdr->name]);
			this->prod = std::string(&strdata[sthdr->prod]);
			if (sthdr->alt) this->alt = std::string(&strdata[sthdr->alt]);

			return nb::StatusCode::SUCCESS;
		}
	};
};

#endif
