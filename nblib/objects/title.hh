#ifndef nblib_objects_title_hh
#define nblib_objects_title_hh

namespace nb
{
	template <typename TString, typename TRawArray>
	struct NbTitle
	{
		u8 seed[16];
		u64 size;
		u64 tid;
		u64 added; /* timestamp */
		u64 updated; /* timestamp */
		u64 dlCount;
		u64 flags;
		u32 id;
		TString name;
		TString alt;
		TString region;
		TString filename;
		TString desc;
		TString prod;
		u16 version;
		u8 contentType; /* piratelegit/legit/stnadard */
		u8 cat;
		u8 subcat;
		bool listed;
		u8 pad[2];
		TRawArray alt_names;
		u32 preferred_alt_idx;
		u8 file_checksum[32];
	};

	using NbTitleRaw = NbTitle<nb::BlobPtr, nb::RawArrayPtr>;

	class Title : public NbTitle<std::string, std::vector<std::string>>
	{
	public:
		static constexpr const char *magic = "TITL";

		nb::StatusCode deserialize(u8 *header, u32 header_size, u8 *blob, u32 blob_size)
		{
			if (header_size < sizeof(NbTitleRaw))
				return nb::StatusCode::INPUT_DATA_TOO_SHORT;

			NbTitleRaw *thdr = reinterpret_cast<NbTitleRaw *>(header);

			this->size = thdr->size;
			this->tid = thdr->tid;
			this->added = thdr->added;
			this->updated = thdr->updated;
			this->dlCount = thdr->dlCount;
			this->flags = thdr->flags;
			this->id = thdr->id;
			this->version = thdr->version;
			this->contentType = thdr->contentType;
			this->listed = thdr->listed;
			this->cat = thdr->cat;
			this->subcat = thdr->subcat;

			memcpy(this->seed, thdr->seed, 16);

			char *strdata = (char *)blob;

			this->name = std::string(&strdata[thdr->name]);
			this->region = std::string(&strdata[thdr->region]);
			this->filename = std::string(&strdata[thdr->filename]);
			this->prod = std::string(&strdata[thdr->prod]);

			if (thdr->desc) this->desc = std::string(&strdata[thdr->desc]);
			if (thdr->alt) this->alt = std::string(&strdata[thdr->alt]);
			if (thdr->alt_names)
			{
				nb::StatusCode c = nb::raw_array::parse_inline<std::string>(this->alt_names, nb::raw_helpers::utf8_str, thdr->alt_names, blob, blob_size);
				if (c != nb::StatusCode::SUCCESS) return c;
			}

			this->preferred_alt_idx = thdr->preferred_alt_idx;
			memcpy(this->file_checksum, thdr->file_checksum, sizeof(thdr->file_checksum));

			return nb::StatusCode::SUCCESS;
		}

		std::string& preferred_alt_name() {
			return this->alt_names[this->preferred_alt_idx];
		}

		bool has_alt() {
			return this->alt_names.size() > 0;
		}
	};
};

#endif
