#ifndef nblib_objects_subcategory_hh
#define nblib_objects_subcategory_hh

namespace nb
{
	template <typename TString>
	struct NbSubcategory
	{
		u32 id;
		TString disp;
		TString name;
		TString desc;
		bool standalone;
	};

	class Subcategory : public NbSubcategory<std::string>
	{
	public:
		static constexpr const char *magic = "SCAT";

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbSubcategory<nb::BlobPtr>))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbSubcategory<nb::BlobPtr> *shdr = (NbSubcategory<nb::BlobPtr> *)header;

			this->id = shdr->id;
			this->standalone = shdr->standalone;

			char *strdata = (char *)blob;

			this->name = std::string(&strdata[shdr->name]);
			this->disp = std::string(&strdata[shdr->disp]);

			if (this->standalone && shdr->desc)
				this->desc = std::string(&strdata[shdr->desc]);

			return nb::StatusCode::SUCCESS;
		}
	};
};
#endif
