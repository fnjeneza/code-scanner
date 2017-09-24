#pragma once

#include "database_generated.h"

namespace code::analyzer {
class serializer
{
  public:
    serializer()  = default;
    ~serializer() = default;

    template <class T> void serialize(const T &data)
    {
        flatbuffers::FlatBufferBuilder        builder(1024);
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
        auto __seq     = builder.CreateVector(sequence);
        auto __usr_seq = Createusr_sequence(builder, __seq);
        builder.Finish(__usr_seq);

        // write file
        std::ofstream out(database_file, std::ios::binary);
        out.write(reinterpret_cast<char *>(builder.GetBufferPointer()),
                  builder.GetSize());
    }

    template <class T> void deserialize(T &data)
    {
        try
        {
            std::ifstream in(database_file, std::ios::binary | std::ios::ate);
            auto          size = in.tellg();
            in.seekg(0);
            std::vector<uint8_t> __dest(size, '\0');
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
        }
        catch (const std::bad_alloc &)
        {
            // TODO log here
        }
    }

  private:
    const std::string database_file{"code-scanner.dbs"};
};
}
