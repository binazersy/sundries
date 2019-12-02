/**=======================================================================

	threadsafe_lookup_table.hpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <exception>
#include <shared_mutex>


template<typename Key, typename Value, typename Hash=std::hash<Key> >
class threadsafe_lookup_table
{
private:
	class bucket_type
	{
	private:
		typedef std::pair<Key, Value> bucket_value;
		typedef std::list<bucket_value> bucket_data;
		typedef typename bucket_data::iterator bucket_iterator;


		bucket_data	 _data;
		//mutable boost::shared_mutex	_mutex;
		mutable std::shared_mutex _mutex;

		bucket_iterator find_entry_for(Key const& key) // const
		{
			return std::find_if(_data.begin(), _data.end(),
				[&](bucket_value const& item) { return item.first == key; });
		}

	public:
		Value value_for(Key const& key, Value const& default_value) //const
		{
			//boost::shared_lock<boost::shared_mutex> lock(_mutex);
			std::shared_lock<std::shared_mutex> lock(_mutex);
			bucket_iterator const found_entry = find_entry_for(key);
			return (found_entry == _data.end()) ? default_value : found_entry->second;
		}

		void add_or_update_mapping(Key const& key, Value const& value)
		{
			//std::unique_lock<boost::shared_mutex> lock(_mutex);
			std::lock_guard<std::shared_mutex> lock(_mutex);
			bucket_iterator const found_entry = find_entry_for(key);
			if (found_entry == _data.end())
			{
				_data.push_back(bucket_value{ key, value });
			}
			else
			{
				found_entry->second = value;
			}
		}

		void remove_mapping(Key const& key)
		{
			//std::unique_lock<boost::shared_mutex> lock(_mutex);
			std::lock_guard<std::shared_mutex> lock(_mutex);
			bucket_iterator const found_entry = find_entry_for(key);
			if (found_entry != _data.end())
			{
				_data.erase(found_entry);
			}
		}
	};

	std::vector<std::unique_ptr<bucket_type> >	_buckets;
	Hash										_hasher;

	bucket_type& get_bucket(Key const& key) const
	{
		std::size_t const index = _hasher(key) % _buckets.size();
		return *_buckets[index];
	}

	bucket_type& get_bucket(size_t i) const
	{
		return *_buckets[i];
	}

public:
	typedef Key key_type;
	typedef Value mapped_type;
	typedef Hash hash_type;

	threadsafe_lookup_table(unsigned num_buckets = 19, Hash const& hasher = Hash())
		: _buckets(num_buckets), _hasher(hasher)
	{
		for (unsigned i = 0; i < num_buckets; ++i)
		{
			_buckets[i].reset(new bucket_type);
		}
	}

	threadsafe_lookup_table(threadsafe_lookup_table const&) = delete;
	threadsafe_lookup_table& operator=(threadsafe_lookup_table const&) = delete;

	Value value_for(Key const& key, Value const& default_value = Value()) const
	{
		return get_bucket(key).value_for(key, default_value);
	}

	void add_or_update_mapping(Key const& key, Value const& value)
	{
		get_bucket(key).add_or_update_mapping(key, value);
	}

	void remove_mapping(Key const& key)
	{
		get_bucket(key).remove_mapping(key);
	}

	std::map<Key, Value> get_map() const
	{
		std::vector<std::lock_guard<std::shared_mutex> > locks;

		for (size_t i = 0; i < _buckets.size(); ++i)
		{
			locks.emplace_back(std::lock_guard<std::shared_mutex>(_buckets[i]._mutex));
		}

		std::map<Key, Value> res;
		for (size_t i = 0; i < _buckets.size(); i++)
		{
			for (typename bucket_type::bucket_iterator it = _buckets[i]->_data.begin(); it != _buckets[i]->_data.end(); it++)
			{
				res.insert(*it);
			}
		}
		return res;
	}
};