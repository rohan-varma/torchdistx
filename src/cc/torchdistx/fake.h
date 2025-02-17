// Copyright (c) Meta Platforms, Inc. and affiliates.
// All rights reserved.
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <memory>

#include <c10/core/DispatchKey.h>
#include <c10/core/Storage.h>

#include "macros.h"

namespace at {

class TensorBase;

}  // namespace at

namespace torchdistx {
namespace detail {

// Since as of this implementation PyTorch is out of dispatch keys we hijack
// the dispatch key of functorch. The implication of this workaround is that
// functorch and fake tensors cannot be used in the same process.
//
// TODO: Once the dispatch key limitation is resolved define our own key.
constexpr auto kFakeDispatchKey = at::DispatchKey::FuncTorchDynamicLayerBackMode;

class FakeTensorImpl;

}  // namespace detail

// Forces all newly-constructed tensors on the calling thread to be fake.
TDX_API void enableFakeMode(bool value);

// Indicates whether `tensor` is fake.
TDX_API bool isFake(const at::TensorBase& tensor) noexcept;

// Provides access to the properties of a fake tensor.
class TDX_API FakeTensor {
 public:
  explicit FakeTensor(const at::TensorBase& tensor, bool unsafe = false);

 public:
  void setData(at::DispatchKey key, std::shared_ptr<void> data);

  bool hasData(at::DispatchKey key) const noexcept;

  std::shared_ptr<void> getData(at::DispatchKey key) const;

  template <typename T>
  inline auto getData(at::DispatchKey key) const {
    return std::static_pointer_cast<T>(getData(key));
  }

  void* unsafeGetData(at::DispatchKey key) const;

  template <typename T>
  inline auto unsafeGetData(at::DispatchKey key) const {
    return static_cast<T*>(unsafeGetData(key));
  }

 public:
  const at::Storage& meta_storage() const noexcept;

 private:
  detail::FakeTensorImpl* impl_;
};

// Treats `tensor` as fake.
TDX_API FakeTensor asFake(const at::TensorBase& tensor);

// Treats `tensor` as fake without performing any type checks.
TDX_API FakeTensor unsafeAsFake(const at::TensorBase& tensor) noexcept;

}  // namespace torchdistx
