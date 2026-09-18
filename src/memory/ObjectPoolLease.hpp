#pragma once

#include <cstddef>
#include <utility>

#include "MemoryTypes.hpp"
#include "detail/ObjectPoolToken.hpp"

namespace ESPressio::Memory {

    /// Move-only RAII ownership of one live object acquired from an Object Pool.
    ///
    /// @tparam TObject Pooled object Type.
    /// @tparam TObjectPool Concrete ObjectPool Type that owns the storage represented by this lease.
    template<class TObject, class TObjectPool>
    class ObjectPoolLease final {

        private:

            // Ownership state.

            /// Non-owning pointer to the Object Pool responsible for this lease.
            TObjectPool* _pool = nullptr;

            /// Compact dedicated/shared allocation token owned by this lease.
            std::size_t _token = Detail::ObjectPoolToken::Empty();

            /// Adopts a newly constructed pooled object.
            void Adopt(
                TObjectPool& pool,
                std::size_t token
            ) noexcept {
                _pool = &pool;
                _token = token;
            }

            template<class, class, class>
            friend class ObjectPool;

        public:

            // Construction and lifetime.

            /// Creates an empty lease owning no object.
            ObjectPoolLease() = default;

            /// Prevents duplicate ownership of one pooled object.
            ObjectPoolLease(const ObjectPoolLease&) = delete;

            /// Prevents duplicate ownership by copy assignment.
            ObjectPoolLease& operator =(const ObjectPoolLease&) = delete;

            /// Transfers ownership from another lease and leaves the source empty.
            ObjectPoolLease(
                ObjectPoolLease&& source
            ) noexcept :
                _pool(source._pool),
                _token(source._token) {
                source._pool = nullptr;
                source._token = Detail::ObjectPoolToken::Empty();
            }

            /// Releases current ownership before transferring ownership from the source lease.
            ObjectPoolLease& operator =(
                ObjectPoolLease&& source
            ) noexcept {
                if (this == &source) { return *this; }

                if (!IsEmpty()) {
                    const auto releaseResult = Release();

                    if (releaseResult == ObjectPoolLeaseReleaseResult::ProviderFailure) {
                        return *this;
                    }
                }

                _pool = source._pool;
                _token = source._token;
                source._pool = nullptr;
                source._token = Detail::ObjectPoolToken::Empty();

                return *this;
            }

            /// Returns owned capacity automatically when this lease leaves scope.
            ~ObjectPoolLease() noexcept {
                if (!IsEmpty()) {
                    static_cast<void>(
                        Release()
                    );
                }
            }


            // Ownership inspection.

            /// Reports whether this lease owns no pooled object.
            bool IsEmpty() const noexcept {
                return _pool == nullptr || Detail::ObjectPoolToken::IsEmpty(_token);
            }

            /// Returns a non-owning pointer valid only while this lease owns a live object.
            TObject* Get() noexcept {
                if (IsEmpty() || Detail::ObjectPoolToken::IsDestroyed(_token)) { return nullptr; }

                return _pool->ResolveToken(_token);
            }

            /// Returns a non-owning pointer valid only while this lease owns a live object.
            const TObject* Get() const noexcept {
                if (IsEmpty() || Detail::ObjectPoolToken::IsDestroyed(_token)) { return nullptr; }

                return _pool->ResolveToken(_token);
            }

            /// Dereferences the live object owned by this lease.
            TObject& operator *() noexcept {
                return *Get();
            }

            /// Dereferences the live object owned by this lease.
            const TObject& operator *() const noexcept {
                return *Get();
            }

            /// Provides member access to the live object owned by this lease.
            TObject* operator ->() noexcept {
                return Get();
            }

            /// Provides member access to the live object owned by this lease.
            const TObject* operator ->() const noexcept {
                return Get();
            }


            // Explicit release.

            /// Destroys the owned object when necessary and returns its capacity to the Object Pool.
            ObjectPoolLeaseReleaseResult Release() noexcept {
                if (IsEmpty()) { return ObjectPoolLeaseReleaseResult::AlreadyEmpty; }

                const auto result = _pool->ReleaseToken(_token);

                if (result == ObjectPoolLeaseReleaseResult::Released) {
                    _pool = nullptr;
                    _token = Detail::ObjectPoolToken::Empty();
                }

                return result;
            }

    };

} // ESPressio::Memory
