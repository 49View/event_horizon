#pragma once

template <typename U, typename K,
        template< typename > typename Sequencer,
        template< typename , typename > typename Container >
class GroupContainer {

protected:
    template< typename KI = K, typename UI = U >
    struct Iterator {
        Iterator(Container<KI, Sequencer<UI>>* _itmap, const bool _begin) : itmap(_itmap) {
            if ( _begin ) {
                itm = itmap->begin();
                ita = itm->second.begin();
            } else {
                itm = itmap->end();
                for ( auto eita = itmap->begin(); eita != itmap->end(); ++eita ) {
                    ita = eita->second.end();
                }
                itmap = nullptr;
            }
        }

        bool operator!=( const Iterator<KI, UI>& _it ) const {
            return itm!= _it.itm || ita != _it.ita || _it.itmap != _it.itmap;
        }

        Iterator<KI, UI>& operator++() {
            if ( ++ita; ita == itm->second.end() ) {
                if ( ++itm; itm == itmap->end() ) {
                    itmap = nullptr;
                } else {
                    ita = itm->second.begin();
                }
            }
            return *this;
        }

        UI operator*() const {
            //assert(itmap);
            return *ita;
        }

        UI& operator*() {
            //assert(itmap);
            return *ita;
        }

        Container<KI, Sequencer<UI>>* itmap = nullptr;
        typename Container<KI, Sequencer<UI>>::iterator itm;
        typename Sequencer<UI>::iterator ita;

    }; // end of Iterator

public:
    Iterator<K, U> begin() {
        return Iterator<K, U>{ &group, true };
    }

    Iterator<K, U> end() {
        return Iterator<K, U>{ &group, false };
    }

protected:
    Container<K, Sequencer<U>> group;
};

template<class T>
using vector_a = std::vector<T>;
template<class T, class T2>
using map_a = std::map<T, T2>;
template<class T, class T2>
using unordered_map_a = std::unordered_map<T, T2>;

template<class T>
using GroupContainerMSV = GroupContainer<T, std::string, vector_a, unordered_map_a>;