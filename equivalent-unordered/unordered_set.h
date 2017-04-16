#include <vector>
#include <cstddef>
#include <functional>

namespace std {
float ceil(float arg);
}

namespace proposed {
template <class Key,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = std::allocator<Key>>
struct unordered_set {
 private:
  using buckets_type = std::vector<std::vector<Key*>>;

 public:
  struct iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = Key;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category = std::forward_iterator_tag;
    iterator()
        : iterator(nullptr,
                   std::numeric_limits<size_type>::max(),
                   std::numeric_limits<size_type>::max()) {}
    iterator(buckets_type const* raw, size_t outerIndex, size_t innerIndex)
        : raw_(raw), outer_(outerIndex), inner_(innerIndex) {}
    reference operator*() const { return *operator->(); }
    pointer operator->() const { return (*raw_)[outer_][inner_]; }
    bool operator==(const iterator& other) {
      return ((raw_ == other.raw_) && (outer_ == other.outer_) &&
              (inner_ == other.inner_));
    }
    iterator& operator++() {
      if (++inner_ < (*raw_)[outer_].size()) {
        return *this;
      }
      if (++outer_ < raw_->size()) {
        inner_ = 0;
        return *this;
      }
      outer_ = std::numeric_limits<size_type>::max();
      inner_ = std::numeric_limits<size_type>::max();
      return *this;
    }
    iterator operator++(int) {
      iterator result{*this};
      operator++();
      return result;
    }

   private:
    buckets_type const* raw_;
    size_t outer_;
    size_t inner_;
    friend struct unordered_set;
  };
  struct local_iterator {
   private:
    using raw_type = typename std::vector<Key const*>::const_iterator;

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = Key;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category = std::forward_iterator_tag;
    local_iterator() : local_iterator({}) {}
    local_iterator(raw_type iter) : iter_(iter) {}
    reference operator*() const { return *operator->(); }
    pointer operator->() const { return (*iter_); }
    bool operator==(const local_iterator& other) {
      return (iter_ == other.iter_);
    }
    iterator& operator++() {
      ++iter_;
      return *this;
    }
    iterator operator++(int) {
      iterator result{*this};
      operator++();
      return result;
    }

   private:
    raw_type iter_;
  };
  using key_type = Key;
  using value_type = Key;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using allocator_type = Allocator;
  using reference = value_type&;
  using const_reference = value_type const&;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using const_iterator = iterator;
  using const_local_iterator = local_iterator;

  unordered_set() : unordered_set(size_type(32)) {}
  explicit unordered_set(size_type bucket_count,
                         const Hash& hash = Hash(),
                         const KeyEqual& equal = KeyEqual(),
                         const Allocator& alloc = Allocator())
      : hash_(hash),
        equal_(equal),
        alloc_(alloc),
        buckets_(bucket_count, alloc) {}
  unordered_set(size_type bucket_count, const Allocator& alloc)
      : unordered_set(bucket_count, Hash(), KeyEqual(), alloc) {}
  unordered_set(size_type bucket_count,
                const Hash& hash,
                const Allocator& alloc)
      : unordered_set(bucket_count, hash, KeyEqual(), alloc) {}
  explicit unordered_set(const Allocator& alloc)
      : unordered_set(size_type(32), alloc) {}
  template <class InputIt>
  unordered_set(InputIt first,
                InputIt last,
                size_type bucket_count = size_type(32),
                const Hash& hash = Hash(),
                const KeyEqual& equal = KeyEqual(),
                const Allocator& alloc = Allocator())
      : unordered_set(bucket_count, hash, equal, alloc) {
    insert(first, last);
  }
  template <class InputIt>
  unordered_set(InputIt first,
                InputIt last,
                size_type bucket_count,
                const Allocator& alloc)
      : unordered_set(first, last, bucket_count, Hash(), KeyEqual(), alloc) {}
  template <class InputIt>
  unordered_set(InputIt first,
                InputIt last,
                size_type bucket_count,
                const Hash& hash,
                const Allocator& alloc)
      : unordered_set(first, last, bucket_count, hash, KeyEqual(), alloc) {}
  unordered_set(const unordered_set& other)
      : unordered_set(other.begin(), other.end()) {}
  unordered_set(const unordered_set& other, const Allocator& alloc)
      : unordered_set(other.begin(), other.end(), size_type(32), alloc) {}
  unordered_set(unordered_set&& other) = default;
  unordered_set(unordered_set&& other, const Allocator& alloc)
      : hash_(std::move(other.hash_)),
        equal_(std::move(other.equal_)),
        alloc_(alloc),
        buckets_(std::move(buckets_)) {
    alloc_ = alloc;
  }
  unordered_set(std::initializer_list<value_type> init,
                size_type bucket_count = size_type(32),
                const Hash& hash = Hash(),
                const KeyEqual& equal = KeyEqual(),
                const Allocator& alloc = Allocator())
      : unordered_set(init.begin(),
                      init.end(),
                      bucket_count,
                      hash,
                      equal,
                      alloc) {}

  unordered_set(std::initializer_list<value_type> init,
                size_type bucket_count,
                const Allocator& alloc)
      : unordered_set(init, bucket_count, Hash(), KeyEqual(), alloc) {}
  unordered_set(std::initializer_list<value_type> init,
                size_type bucket_count,
                const Hash& hash,
                const Allocator& alloc)
      : unordered_set(init, bucket_count, hash, KeyEqual(), alloc) {}

  ~unordered_set() { clear(); }

  unordered_set& operator=(const unordered_set& other) {
    clear();
    hash_ = other.hash_;
    equal_ = other.equal_;
    alloc_ = other.alloc_;
    insert(other.begin(), other.end());
    return *this;
  }
  unordered_set& operator=(unordered_set&& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value&&
          std::is_nothrow_move_assignable<Hash>::value&&
              std::is_nothrow_move_assignable<KeyEqual>::value) {
    hash_ = std::move(other.hash_);
    equal_ = std::move(other.equal_);
    alloc_ = std::move(other.alloc_);
    buckets_ = std::move(other.buckets_);
    return *this;
  }

  unordered_set& operator=(std::initializer_list<value_type> ilist) {
    clear();
    insert(ilist.begin(), ilist.end());
    return *this;
  }

  allocator_type get_allocator() const { return alloc_; }

  iterator begin() { return cbegin(); }
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const {
    auto outer = buckets_.begin();
    do {
      auto inner = outer->begin();
      if (inner != outer->end()) {
        return {buckets_, outer, inner};
      }
    } while (++outer != buckets_.end());
    return {};
  }

  iterator end() { return cend(); }
  const_iterator end() const { return cend(); }
  const_iterator cend() const { return {}; }

  bool empty() const {
    for (auto const& bucket : buckets_) {
      if (!bucket.empty()) {
        return false;
      }
    }
    return true;
  }

  size_type size() const {
    size_type result{};
    for (auto const& bucket : buckets_) {
      result += bucket.size();
    }
    return result;
  }

  size_type max_size() const { return std::numeric_limits<size_type>::max(); }

  void clear() {
    for (auto& bucket : buckets_) {
      for (auto& node : bucket) {
        lose(node);
      }
      bucket.clear();
    }
  }

  void swap(unordered_set& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value&&
          std::is_nothrow_swappable<Hash>::value&&
              std::is_nothrow_swappable<KeyEqual>::value) {
    swap(hash_, other.hash_);
    swap(equal_, other.equal_);
    if (std::allocator_traits<
            allocator_type>::propagate_on_container_swap::value) {
      swap(alloc_, other.alloc_);
    }
    swap(buckets_, other.buckets_);
  }

 private:
  template <typename VT>
  std::pair<iterator, bool> insert_helper(VT&& vt) {
    auto bucketIndex = bucket(vt);
    auto& bucket = buckets_[bucketIndex];
    auto bucketSize = bucket.size();
    size_t entryIndex{};
    for (; entryIndex < bucketSize; ++entryIndex) {
      if (equal_(*bucket[entryIndex], vt)) {
        return {iterator{&buckets_, bucketIndex, entryIndex}, false};
      }
    }
    auto newEntryPtr =
        std::allocator_traits<allocator_type>::allocate(alloc_, 1);
    std::allocator_traits<allocator_type>::construct(alloc_, newEntryPtr,
                                                     std::forward<VT>(vt));
    bucket.emplace_back(newEntryPtr);
    return {iterator{&buckets_, bucketIndex, entryIndex}, true};
  }

  template <typename VT>
  iterator insert_helper(const_iterator hint, VT&& vt) {
    auto bucketIndex = bucket(vt);
    auto& bucket = buckets_[bucketIndex];
    auto bucketSize = bucket.size();
    size_t entryIndex{};
    for (; entryIndex < bucketSize; ++entryIndex) {
      if (equal_to(*bucket[entryIndex], vt)) {
        return {&buckets_, bucketIndex, entryIndex};
      }
    }
    auto newEntryPtr =
        std::allocator_traits<allocator_type>::allocate(alloc_, 1);
    std::allocator_traits<allocator_type>::construct(alloc_, newEntryPtr,
                                                     std::forward<VT>(vt));
    if (hint.outer_ == bucketIndex) {
      bucket.insert(hint.inner_, newEntryPtr);
      return hint;
    }
    if (hint.outer_ >= bucketIndex) {
      bucket.emplace_back(newEntryPtr);
      return {&buckets_, bucketIndex, entryIndex};
    }
    bucket.insert(0, newEntryPtr);
    return {&buckets_, bucketIndex, 0};
  }
  void lose(Key* thing) {
    std::allocator_traits<allocator_type>::destroy(alloc_, thing);
    std::allocator_traits<allocator_type>::deallocate(alloc_, thing, 1);
  }

 public:
  std::pair<iterator, bool> insert(const value_type& value) {
    return insert_helper(value);
  }
  std::pair<iterator, bool> insert(value_type&& value) {
    return insert_helper(std::move(value));
  }
  iterator insert(const_iterator hint, const value_type& value) {
    return insert_helper(hint, value);
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return insert_helper(hint, std::move(value));
  }
  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    while (first != last) {
      insert(*first);
      ++first;
    }
  }
  void insert(std::initializer_list<value_type> ilist) {
    for (auto const& key : ilist) {
      insert(key);
    }
  }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    auto newEntryPtr =
        std::allocator_traits<allocator_type>::allocate(alloc_, 1);
    std::allocator_traits<allocator_type>::construct(
        alloc_, newEntryPtr, std::forward<Args>(args)...);
    auto bucketIndex = bucket(*newEntryPtr);
    auto& bucket = buckets_[bucketIndex];
    auto bucketSize = bucket.size();
    size_t entryIndex{};
    for (; entryIndex < bucketSize; ++entryIndex) {
      if (equal_to(*bucket[entryIndex], *newEntryPtr)) {
        lose(newEntryPtr);
        return {iterator{&buckets_, bucketIndex, entryIndex}, false};
      }
    }
    bucket.emplace_back(newEntryPtr);
    return {iterator{&buckets_, bucketIndex, entryIndex}, true};
  }

  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    auto newEntryPtr =
        std::allocator_traits<allocator_type>::allocate(alloc_, 1);
    std::allocator_traits<allocator_type>::construct(
        alloc_, newEntryPtr, std::forward<Args>(args)...);
    auto bucketIndex = bucket(*newEntryPtr);
    auto& bucket = buckets_[bucketIndex];
    auto bucketSize = bucket.size();
    size_t entryIndex{};
    for (; entryIndex < bucketSize; ++entryIndex) {
      if (equal_to(*bucket[entryIndex], *newEntryPtr)) {
        lose(newEntryPtr);
        return {&buckets_, bucketIndex, entryIndex};
      }
    }
    if (hint.outer_ == bucketIndex) {
      bucket.insert(hint.inner_, newEntryPtr);
      return hint;
    }
    if (hint.outer_ >= bucketIndex) {
      bucket.emplace_back(newEntryPtr);
      return {&buckets_, bucketIndex, entryIndex};
    }
    bucket.insert(0, newEntryPtr);
    return {&buckets_, bucketIndex, 0};
  }

  iterator erase(const_iterator pos) {
    lose(buckets_[pos.outer_][pos.inner_]);
    buckets_[pos.outer_].erase(buckets_[pos.outer_].begin() + pos.inner_);
    if (buckets_[pos.outer_].size() > pos.inner_) {
      return pos;
    }
    return ++pos;
  }

  iterator erase(const_iterator first, const_iterator last) {
    auto cursor = first;
    while (cursor != last) {
      lose(buckets_[cursor.outer_][cursor.inner_]);
      ++cursor;
    }
    lose(buckets_[cursor.outer_][cursor.inner_]);
    cursor = first;
    while (cursor.outer_ != last.outer_) {
      auto& bucket = buckets_[cursor.outer_];
      bucket.erase(bucket.begin() + cursor.inner_, bucket.end());
      ++cursor;
    }
    auto& bucket = buckets_[cursor.outer_];
    bucket.erase(bucket.begin() + cursor.inner_, bucket.begin() + last.inner_);
    return ++cursor;
  }

  size_type erase(const key_type& key) {
    auto iter = find(key);
    if (iter == end()) {
      return 0;
    }
    erase(iter);
    return 1;
  }

  size_type count(const Key& key) const {
    auto iter = find(key);
    if (iter == end()) {
      return 0;
    }
    return 1;
  }

  const_iterator find(const Key& key) const {
    auto bucketIndex = bucket(key);
    auto& bucket = buckets_[bucketIndex];
    auto bucketSize = bucket.size();
    size_t entryIndex{};
    for (; entryIndex < bucketSize; ++entryIndex) {
      if (equal_(*bucket[entryIndex], key)) {
        return {&buckets_, bucketIndex, entryIndex};
      }
    }
    return end();
  }

  std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
    auto iter = find(key);
    return {iter, iter};
  }

  const_local_iterator begin(size_type n) const { return cbegin(n); }
  const_local_iterator cbegin(size_type n) const { return buckets_[n].begin(); }

  const_local_iterator end(size_type n) const { return cend(n); }
  const_local_iterator cend(size_type n) const { return buckets_[n].end(); }

  size_type bucket_count() const { return buckets_.size(); }

  size_type max_bucket_count() const {
    return std::numeric_limits<size_type>::max();
  }

  size_type bucket_size(size_type n) const { return buckets_[n].size(); }

  size_type bucket(const Key& key) const {
    return hash_(key) % buckets_.size();
  }

  float load_factor() const {
    float result = size();
    return result / bucket_count();
  }

  hasher hash_function() const { return hash_; }

  key_equal key_eq() const { return equal_; }

  float max_load_factor() const { return max_load_factor_; }

  void max_load_factor(float ml) { max_load_factor_ = std::max(1.0f, ml); }

 private:
  void actually_rehash(size_type count) {
    std::vector<std::vector<Key*>> oldBuckets{std::move(buckets_)};
    buckets_ = std::vector<std::vector<Key*>>(count);
    for (auto& oldBucket : oldBuckets) {
      for (auto ptr : oldBucket) {
        buckets_[bucket(*ptr)].emplace_back(ptr);
      }
    }
  }

 public:
  void rehash(size_type count) {
    size_type actual_count = count;
    auto current_size = size();
    if (actual_count == 0) {
      actual_count = ceilf(1.5f * current_size / max_load_factor_);
    } else {
      float resulting_load_factor = current_size;
      resulting_load_factor /= count;
      if (resulting_load_factor > max_load_factor_) {
        actual_count = ceilf(1.5f * current_size / max_load_factor_);
      }
    }
    actually_rehash(actual_count);
  }

  void reserve(size_type count) {
    actually_rehash(std::ceil(std::max(count, size()) / max_load_factor_));
  }

 private:
  Hash hash_;
  KeyEqual equal_;
  Allocator alloc_;
  buckets_type buckets_;
  float max_load_factor_;
};

template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator==(const unordered_set<Key, Hash, KeyEqual, Allocator>& lhs,
                const unordered_set<Key, Hash, KeyEqual, Allocator>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (auto const& key : lhs) {
    if (rhs.count(key) == 0) {
      return false;
    }
  }
  return true;
}

template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator!=(const unordered_set<Key, Hash, KeyEqual, Allocator>& lhs,
                const unordered_set<Key, Hash, KeyEqual, Allocator>& rhs) {
  return !operator==(lhs, rhs);
}

template <class Key, class Hash, class KeyEqual, class Alloc>
void swap(unordered_set<Key, Hash, KeyEqual, Alloc>& lhs,
          unordered_set<Key, Hash, KeyEqual, Alloc>&
              rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}
}
