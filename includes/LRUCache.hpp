#include <list>
#include <unordered_map>
#include <assert.h>


/**
 * original from Tsuneo Yoshioka on https://stackoverflow.com/questions/2504178/lru-cache-design/14503492#14503492
 * with some modifications
 */
template <class KEY_T, class VAL_T> class LRUCache{
public:
		typedef std::list< std::pair<KEY_T,VAL_T> > list_type;
		typedef typename std::unordered_map<KEY_T, typename list_type::iterator> map_type;
private:
		list_type item_list;
		map_type item_map;
		std::size_t cache_size;
private:
		void clean(void){
				while(item_map.size()>cache_size) {
						auto last_it = item_list.end(); last_it --;
						item_map.erase(last_it->first);
						item_list.pop_back();
				}
		}
public:
		LRUCache(int cache_size_):cache_size(cache_size_) { }

		void put(const KEY_T &key, const VAL_T &val) {
				auto it = item_map.find(key);
				if(it != item_map.end()){
						item_list.erase(it->second);
						item_map.erase(it);
				}
				item_list.push_front(make_pair(key,val));
				item_map.insert(make_pair(key, item_list.begin()));
				clean();
		}
		typename list_type::const_iterator get(const KEY_T &key) {
				auto it = item_map.find(key);
				if (it == item_map.end())
					return item_list.end();
				item_list.splice(item_list.begin(), item_list, it->second);
				return it->second;
		}
		typename list_type::const_iterator end() const { return item_list.end(); }
		template <typename F>
		void map(F f) {
			map_type new_map;
			for (auto it = item_list.rbegin(); it != item_list.rend(); ++it) {
				f(*it);
				auto it2 = it;
				++it2;
				new_map[it->first] = (it2).base();
			}
			item_map = std::move(new_map);
		}
		void clear() {
			item_list.clear();
			item_map.clear();
		}

};
