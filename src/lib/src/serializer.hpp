#pragma once

#include "database_generated.h"
#include "file.hpp"

namespace code::analyzer {
class serializer
{
  public:
    serializer()  = default;
    ~serializer() = default;

    template <class T, class F> void serialize(const T &data, const F &files)
    {
        flatbuffers::FlatBufferBuilder builder(1024);
        auto sequence     = usrs_to_flatbuffers_vector(builder, data);
        auto __files      = files_to_flatbuffers_vector(builder, files);
        auto __usrs_list  = builder.CreateVector(sequence);
        auto __files_list = builder.CreateVector(__files);
        auto __usr_seq = Createusr_sequence(builder, __usrs_list, __files_list);
        builder.Finish(__usr_seq);

        // write file
        std::ofstream out(database_file, std::ios::binary);
        out.write(reinterpret_cast<char *>(builder.GetBufferPointer()),
                  builder.GetSize());
    }

    template <class T, class F> void deserialize(T &data, F &files)
    {
        std::ifstream in(database_file, std::ios::binary | std::ios::ate);
        auto          size = in.tellg();
        if (size <= 0)
        {
            return;
        }
        in.seekg(0);
        std::vector<uint8_t> __dest(static_cast<std::size_t>(size), '\0');
        in.read(reinterpret_cast<char *>(__dest.data()), size);
        auto __usr_seq = Getusr_sequence(__dest.data());

        // TODO need optimization
        for (auto v : *__usr_seq->usrs())
        {
            std::set<std::string> defs;
            for (auto def : *v->definitions())
            {
                defs.emplace(def->c_str());
            }

            data.emplace(v->name()->c_str(), defs);
        }

        for (auto f : *__usr_seq->files())
        {
            utils::File __file(f->path()->c_str(), f->timestamp());
            files.emplace(__file);
        }
    }

  private:
    template <class T>
    std::vector<flatbuffers::Offset<USR>>
    usrs_to_flatbuffers_vector(flatbuffers::FlatBufferBuilder &builder,
                               const T &                       data)
    {
        std::vector<flatbuffers::Offset<USR>> sequence;
        for (auto &d : data)
        {
            auto key = builder.CreateString(d.first);
            std::vector<flatbuffers::Offset<flatbuffers::String>> definitions;
            for (auto &v : d.second)
            {
                auto value = builder.CreateString(v);
                definitions.push_back(value);
            }
            auto defs = builder.CreateVector(definitions);
            auto usr  = CreateUSR(builder, key, defs);
            sequence.push_back(usr);
        }
        return sequence;
    }

    template <class T>
    std::vector<flatbuffers::Offset<File>>
    files_to_flatbuffers_vector(flatbuffers::FlatBufferBuilder &builder,
                                const T &                       files)
    {
        std::vector<flatbuffers::Offset<File>> sequence;
        for (auto &f : files)
        {
            auto __path = builder.CreateString(f.path());
            auto usr    = CreateFile(builder, __path, f.timestamp());
            sequence.push_back(usr);
        }
        return sequence;
    }

  private:
    const std::string database_file{"code-scanner.db"};
};
}
