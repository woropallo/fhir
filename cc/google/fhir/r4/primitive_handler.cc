/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "google/fhir/r4/primitive_handler.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include "google/protobuf/descriptor.h"
#include "absl/memory/memory.h"
#include "absl/strings/string_view.h"
#include "google/fhir/annotations.h"
#include "google/fhir/codes.h"
#include "google/fhir/extensions.h"
#include "google/fhir/fhir_types.h"
#include "google/fhir/primitive_handler.h"
#include "google/fhir/primitive_wrapper.h"
#include "google/fhir/proto_util.h"
#include "google/fhir/status/status.h"
#include "google/fhir/status/statusor.h"
#include "google/fhir/util.h"
#include "proto/r4/core/datatypes.pb.h"
#include "proto/r4/google_extensions.pb.h"
#include "include/json/json.h"
#include "tensorflow/core/lib/core/errors.h"

namespace google {
namespace fhir {
namespace r4 {

using primitives_internal::PrimitiveWrapper;
using primitives_internal::StringTypeWrapper;
using ::google::protobuf::Descriptor;
using ::tensorflow::errors::InvalidArgument;

StatusOr<std::unique_ptr<PrimitiveWrapper>> R4PrimitiveHandler::GetWrapper(
    const Descriptor* target_descriptor) const {
  absl::optional<std::unique_ptr<PrimitiveWrapper>> common_wrapper =
      primitives_internal::GetCommonWrapper<
          core::Extension, core::Xhtml, google::Base64BinarySeparatorStride>(
          target_descriptor);

  if (common_wrapper.has_value()) {
    return std::move(common_wrapper.value());
  }

  if (IsMessageType<core::Canonical>(target_descriptor)) {
    return std::unique_ptr<PrimitiveWrapper>(
        new StringTypeWrapper<core::Canonical>());
  }

  if (IsMessageType<core::Url>(target_descriptor)) {
    return std::unique_ptr<PrimitiveWrapper>(
        new StringTypeWrapper<core::Url>());
  }

  return InvalidArgument("Unexpected R4 primitive FHIR type: ",
                         target_descriptor->full_name());
}

const R4PrimitiveHandler* R4PrimitiveHandler::GetInstance() {
  static R4PrimitiveHandler* instance = new R4PrimitiveHandler;
  return instance;
}

}  // namespace r4
}  // namespace fhir
}  // namespace google
