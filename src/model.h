#pragma once

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>

//namespace pt = boost::property_tree;

namespace sagacious
{
    template <class T> class model
    {
    public:
        model();
        virtual ~model();

        void save();
        void remove();

        static T get(std::string id);

    protected:
        mongocxx::collection collection() const;

    private:
        static thread_local mongocxx::client client;
    };

    template <class T> model<T>::model()
    {
    }

    template <class T> model<T>::~model()
    {
    }

    template <class T> void model<T>::save()
    {
    }

    template <class T> void model<T>::remove()
    {
    }

    using bsoncxx::builder::basic::kvp;

    template <class T> T model<T>::get(std::string id)
    {
        T obj{};
        bsoncxx::oid oid{id};

        auto collection = obj.collection();
        auto document = bsoncxx::builder::basic::make_document(kvp("_id", oid));
        auto result   = collection.find_one(document.view());

        if (result) {
            std::cout << "ok" << std::endl;
            //
        } else {
            std::cout << "nope" << std::endl;
            //
        }

//        pt::ptree root;
//        boost::property_tree::read_json(bsoncxx::to_json(result.value()), root);

        return obj;
    }

    template <class T> mongocxx::collection model<T>::collection() const
    {
    }

    template <class T>
    thread_local mongocxx::client model<T>::client{mongocxx::uri{}};
}
