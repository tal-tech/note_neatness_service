# GRPC global cmake file
# This currently builds C and C++ code.
# This file has been automatically generated from a template file.
# Please look at the templates directory instead.
# This file can be regenerated from the template by running
# tools/buildgen/generate_projects.sh
#
# Copyright 2015 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



cmake_minimum_required(VERSION 2.8)

set(PACKAGE_NAME      "grpc")
set(PACKAGE_VERSION   "1.9.1")
set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/grpc/grpc/issues/")
project(${PACKAGE_NAME} C CXX)

set(gRPC_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(gRPC_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(gRPC_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(gRPC_INSTALL_CMAKEDIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/${PACKAGE_NAME}" CACHE PATH "Installation directory for cmake config files")
set(BUILD_SHARED_LIBS ON)
# Options
option(gRPC_BUILD_TESTS "Build tests" OFF)

set(gRPC_INSTALL_default ON)
if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Disable gRPC_INSTALL by default if building as a submodule
  set(gRPC_INSTALL_default OFF)
endif()
set(gRPC_INSTALL ${gRPC_INSTALL_default} CACHE BOOL
    "Generate installation target: gRPC_ZLIB_PROVIDER, gRPC_CARES_PROVIDER, gRPC_SSL_PROVIDER and gRPC_PROTOBUF_PROVIDER must all be \"package\"")

# Providers for third-party dependencies (gRPC_*_PROVIDER properties):
# "module": build the dependency using sources from git submodule (under third_party)
# "package": use cmake's find_package functionality to locate a pre-installed dependency

set(gRPC_ZLIB_PROVIDER "module" CACHE STRING "Provider of zlib library")
set_property(CACHE gRPC_ZLIB_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_CARES_PROVIDER "module" CACHE STRING "Provider of c-ares library")
set_property(CACHE gRPC_CARES_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_SSL_PROVIDER "module" CACHE STRING "Provider of ssl library")
set_property(CACHE gRPC_SSL_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_PROTOBUF_PROVIDER "module" CACHE STRING "Provider of protobuf library")
set_property(CACHE gRPC_PROTOBUF_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_PROTOBUF_PACKAGE_TYPE "" CACHE STRING "Algorithm for searching protobuf package")
set_property(CACHE gRPC_PROTOBUF_PACKAGE_TYPE PROPERTY STRINGS "CONFIG" "MODULE")

set(gRPC_GFLAGS_PROVIDER "module" CACHE STRING "Provider of gflags library")
set_property(CACHE gRPC_GFLAGS_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_BENCHMARK_PROVIDER "module" CACHE STRING "Provider of benchmark library")
set_property(CACHE gRPC_BENCHMARK_PROVIDER PROPERTY STRINGS "module" "package")

set(gRPC_USE_PROTO_LITE OFF CACHE BOOL "Use the protobuf-lite library")

if(UNIX)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(_gRPC_PLATFORM_LINUX ON)
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(_gRPC_PLATFORM_MAC ON)
  else()
    set(_gRPC_PLATFORM_POSIX ON)
  endif()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)


if (gRPC_USE_PROTO_LITE)
  set(_gRPC_PROTOBUF_LIBRARY_NAME "libprotobuf-lite")
  add_definitions("-DGRPC_USE_PROTO_LITE")
else()
  set(_gRPC_PROTOBUF_LIBRARY_NAME "libprotobuf")
endif()

include(cmake/zlib.cmake)
include(cmake/cares.cmake)
include(cmake/protobuf.cmake)
include(cmake/ssl.cmake)
include(cmake/gflags.cmake)
#include(cmake/benchmark.cmake)

if(NOT MSVC)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -std=c99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


# Create directory for generated .proto files
set(_gRPC_PROTO_GENS_DIR ${CMAKE_BINARY_DIR}/gens)
file(MAKE_DIRECTORY ${_gRPC_PROTO_GENS_DIR})

#  protobuf_generate_grpc_cpp
#  --------------------------
#
#   Add custom commands to process ``.proto`` files to C++ using protoc and
#   GRPC plugin::
#
#     protobuf_generate_grpc_cpp [<ARGN>...]
#
#   ``ARGN``
#     ``.proto`` files
#
function(protobuf_generate_grpc_cpp)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_GRPC_CPP() called without any proto files")
    return()
  endif()

  set(_protobuf_include_path -I . -I ${_gRPC_PROTOBUF_WELLKNOWN_INCLUDE_DIR})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    file(RELATIVE_PATH REL_FIL ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FIL})
    get_filename_component(REL_DIR ${REL_FIL} DIRECTORY)
    set(RELFIL_WE "${REL_DIR}/${FIL_WE}")

    add_custom_command(
      OUTPUT "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.cc"
             "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.h"
             "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}_mock.grpc.pb.h"
             "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.cc"
             "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.h"
      COMMAND ${_gRPC_PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --grpc_out=generate_mock_code=true:${_gRPC_PROTO_GENS_DIR}
           --cpp_out=${_gRPC_PROTO_GENS_DIR}
           --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
           ${_protobuf_include_path}
           ${REL_FIL}
      DEPENDS ${ABS_FIL} ${_gRPC_PROTOBUF_PROTOC} grpc_cpp_plugin
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
      VERBATIM)

      set_source_files_properties("${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.cc" "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.h"  "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}_mock.grpc.pb.h" "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.cc" "${_gRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.h" PROPERTIES GENERATED TRUE)
  endforeach()
endfunction()

add_custom_target(plugins
  DEPENDS
  grpc_cpp_plugin
)

add_library(gpr
  src/core/lib/gpr/alloc.cc
  src/core/lib/gpr/arena.cc
  src/core/lib/gpr/atm.cc
  src/core/lib/gpr/avl.cc
  src/core/lib/gpr/cmdline.cc
  src/core/lib/gpr/cpu_iphone.cc
  src/core/lib/gpr/cpu_linux.cc
  src/core/lib/gpr/cpu_posix.cc
  src/core/lib/gpr/cpu_windows.cc
  src/core/lib/gpr/env_linux.cc
  src/core/lib/gpr/env_posix.cc
  src/core/lib/gpr/env_windows.cc
  src/core/lib/gpr/fork.cc
  src/core/lib/gpr/host_port.cc
  src/core/lib/gpr/log.cc
  src/core/lib/gpr/log_android.cc
  src/core/lib/gpr/log_linux.cc
  src/core/lib/gpr/log_posix.cc
  src/core/lib/gpr/log_windows.cc
  src/core/lib/gpr/mpscq.cc
  src/core/lib/gpr/murmur_hash.cc
  src/core/lib/gpr/string.cc
  src/core/lib/gpr/string_posix.cc
  src/core/lib/gpr/string_util_windows.cc
  src/core/lib/gpr/string_windows.cc
  src/core/lib/gpr/subprocess_posix.cc
  src/core/lib/gpr/subprocess_windows.cc
  src/core/lib/gpr/sync.cc
  src/core/lib/gpr/sync_posix.cc
  src/core/lib/gpr/sync_windows.cc
  src/core/lib/gpr/thd.cc
  src/core/lib/gpr/thd_posix.cc
  src/core/lib/gpr/thd_windows.cc
  src/core/lib/gpr/time.cc
  src/core/lib/gpr/time_posix.cc
  src/core/lib/gpr/time_precise.cc
  src/core/lib/gpr/time_windows.cc
  src/core/lib/gpr/tls_pthread.cc
  src/core/lib/gpr/tmpfile_msys.cc
  src/core/lib/gpr/tmpfile_posix.cc
  src/core/lib/gpr/tmpfile_windows.cc
  src/core/lib/gpr/wrap_memcpy.cc
  src/core/lib/profiling/basic_timers.cc
  src/core/lib/profiling/stap_timers.cc
)



target_include_directories(gpr
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
)

target_link_libraries(gpr
  ${_gRPC_ALLTARGETS_LIBRARIES}
)

set_target_properties(gpr PROPERTIES SOVERSION 6)


add_library(grpc
  src/core/lib/surface/init.cc
  src/core/lib/backoff/backoff.cc
  src/core/lib/channel/channel_args.cc
  src/core/lib/channel/channel_stack.cc
  src/core/lib/channel/channel_stack_builder.cc
  src/core/lib/channel/connected_channel.cc
  src/core/lib/channel/handshaker.cc
  src/core/lib/channel/handshaker_factory.cc
  src/core/lib/channel/handshaker_registry.cc
  src/core/lib/compression/compression.cc
  src/core/lib/compression/message_compress.cc
  src/core/lib/compression/stream_compression.cc
  src/core/lib/compression/stream_compression_gzip.cc
  src/core/lib/compression/stream_compression_identity.cc
  src/core/lib/debug/stats.cc
  src/core/lib/debug/stats_data.cc
  src/core/lib/http/format_request.cc
  src/core/lib/http/httpcli.cc
  src/core/lib/http/parser.cc
  src/core/lib/iomgr/call_combiner.cc
  src/core/lib/iomgr/combiner.cc
  src/core/lib/iomgr/endpoint.cc
  src/core/lib/iomgr/endpoint_pair_posix.cc
  src/core/lib/iomgr/endpoint_pair_uv.cc
  src/core/lib/iomgr/endpoint_pair_windows.cc
  src/core/lib/iomgr/error.cc
  src/core/lib/iomgr/ev_epoll1_linux.cc
  src/core/lib/iomgr/ev_epollex_linux.cc
  src/core/lib/iomgr/ev_epollsig_linux.cc
  src/core/lib/iomgr/ev_poll_posix.cc
  src/core/lib/iomgr/ev_posix.cc
  src/core/lib/iomgr/ev_windows.cc
  src/core/lib/iomgr/exec_ctx.cc
  src/core/lib/iomgr/executor.cc
  src/core/lib/iomgr/fork_posix.cc
  src/core/lib/iomgr/fork_windows.cc
  src/core/lib/iomgr/gethostname_fallback.cc
  src/core/lib/iomgr/gethostname_host_name_max.cc
  src/core/lib/iomgr/gethostname_sysconf.cc
  src/core/lib/iomgr/iocp_windows.cc
  src/core/lib/iomgr/iomgr.cc
  src/core/lib/iomgr/iomgr_posix.cc
  src/core/lib/iomgr/iomgr_uv.cc
  src/core/lib/iomgr/iomgr_windows.cc
  src/core/lib/iomgr/is_epollexclusive_available.cc
  src/core/lib/iomgr/load_file.cc
  src/core/lib/iomgr/lockfree_event.cc
  src/core/lib/iomgr/network_status_tracker.cc
  src/core/lib/iomgr/polling_entity.cc
  src/core/lib/iomgr/pollset_set_uv.cc
  src/core/lib/iomgr/pollset_set_windows.cc
  src/core/lib/iomgr/pollset_uv.cc
  src/core/lib/iomgr/pollset_windows.cc
  src/core/lib/iomgr/resolve_address_posix.cc
  src/core/lib/iomgr/resolve_address_uv.cc
  src/core/lib/iomgr/resolve_address_windows.cc
  src/core/lib/iomgr/resource_quota.cc
  src/core/lib/iomgr/sockaddr_utils.cc
  src/core/lib/iomgr/socket_factory_posix.cc
  src/core/lib/iomgr/socket_mutator.cc
  src/core/lib/iomgr/socket_utils_common_posix.cc
  src/core/lib/iomgr/socket_utils_linux.cc
  src/core/lib/iomgr/socket_utils_posix.cc
  src/core/lib/iomgr/socket_utils_uv.cc
  src/core/lib/iomgr/socket_utils_windows.cc
  src/core/lib/iomgr/socket_windows.cc
  src/core/lib/iomgr/tcp_client_posix.cc
  src/core/lib/iomgr/tcp_client_uv.cc
  src/core/lib/iomgr/tcp_client_windows.cc
  src/core/lib/iomgr/tcp_posix.cc
  src/core/lib/iomgr/tcp_server_posix.cc
  src/core/lib/iomgr/tcp_server_utils_posix_common.cc
  src/core/lib/iomgr/tcp_server_utils_posix_ifaddrs.cc
  src/core/lib/iomgr/tcp_server_utils_posix_noifaddrs.cc
  src/core/lib/iomgr/tcp_server_uv.cc
  src/core/lib/iomgr/tcp_server_windows.cc
  src/core/lib/iomgr/tcp_uv.cc
  src/core/lib/iomgr/tcp_windows.cc
  src/core/lib/iomgr/time_averaged_stats.cc
  src/core/lib/iomgr/timer_generic.cc
  src/core/lib/iomgr/timer_heap.cc
  src/core/lib/iomgr/timer_manager.cc
  src/core/lib/iomgr/timer_uv.cc
  src/core/lib/iomgr/udp_server.cc
  src/core/lib/iomgr/unix_sockets_posix.cc
  src/core/lib/iomgr/unix_sockets_posix_noop.cc
  src/core/lib/iomgr/wakeup_fd_cv.cc
  src/core/lib/iomgr/wakeup_fd_eventfd.cc
  src/core/lib/iomgr/wakeup_fd_nospecial.cc
  src/core/lib/iomgr/wakeup_fd_pipe.cc
  src/core/lib/iomgr/wakeup_fd_posix.cc
  src/core/lib/json/json.cc
  src/core/lib/json/json_reader.cc
  src/core/lib/json/json_string.cc
  src/core/lib/json/json_writer.cc
  src/core/lib/slice/b64.cc
  src/core/lib/slice/percent_encoding.cc
  src/core/lib/slice/slice.cc
  src/core/lib/slice/slice_buffer.cc
  src/core/lib/slice/slice_hash_table.cc
  src/core/lib/slice/slice_intern.cc
  src/core/lib/slice/slice_string_helpers.cc
  src/core/lib/surface/alarm.cc
  src/core/lib/surface/api_trace.cc
  src/core/lib/surface/byte_buffer.cc
  src/core/lib/surface/byte_buffer_reader.cc
  src/core/lib/surface/call.cc
  src/core/lib/surface/call_details.cc
  src/core/lib/surface/call_log_batch.cc
  src/core/lib/surface/channel.cc
  src/core/lib/surface/channel_init.cc
  src/core/lib/surface/channel_ping.cc
  src/core/lib/surface/channel_stack_type.cc
  src/core/lib/surface/completion_queue.cc
  src/core/lib/surface/completion_queue_factory.cc
  src/core/lib/surface/event_string.cc
  src/core/lib/surface/lame_client.cc
  src/core/lib/surface/metadata_array.cc
  src/core/lib/surface/server.cc
  src/core/lib/surface/validate_metadata.cc
  src/core/lib/surface/version.cc
  src/core/lib/transport/bdp_estimator.cc
  src/core/lib/transport/byte_stream.cc
  src/core/lib/transport/connectivity_state.cc
  src/core/lib/transport/error_utils.cc
  src/core/lib/transport/metadata.cc
  src/core/lib/transport/metadata_batch.cc
  src/core/lib/transport/pid_controller.cc
  src/core/lib/transport/service_config.cc
  src/core/lib/transport/static_metadata.cc
  src/core/lib/transport/status_conversion.cc
  src/core/lib/transport/timeout_encoding.cc
  src/core/lib/transport/transport.cc
  src/core/lib/transport/transport_op_string.cc
  src/core/lib/debug/trace.cc
  src/core/ext/transport/chttp2/server/secure/server_secure_chttp2.cc
  src/core/ext/transport/chttp2/transport/bin_decoder.cc
  src/core/ext/transport/chttp2/transport/bin_encoder.cc
  src/core/ext/transport/chttp2/transport/chttp2_plugin.cc
  src/core/ext/transport/chttp2/transport/chttp2_transport.cc
  src/core/ext/transport/chttp2/transport/flow_control.cc
  src/core/ext/transport/chttp2/transport/frame_data.cc
  src/core/ext/transport/chttp2/transport/frame_goaway.cc
  src/core/ext/transport/chttp2/transport/frame_ping.cc
  src/core/ext/transport/chttp2/transport/frame_rst_stream.cc
  src/core/ext/transport/chttp2/transport/frame_settings.cc
  src/core/ext/transport/chttp2/transport/frame_window_update.cc
  src/core/ext/transport/chttp2/transport/hpack_encoder.cc
  src/core/ext/transport/chttp2/transport/hpack_parser.cc
  src/core/ext/transport/chttp2/transport/hpack_table.cc
  src/core/ext/transport/chttp2/transport/http2_settings.cc
  src/core/ext/transport/chttp2/transport/huffsyms.cc
  src/core/ext/transport/chttp2/transport/incoming_metadata.cc
  src/core/ext/transport/chttp2/transport/parsing.cc
  src/core/ext/transport/chttp2/transport/stream_lists.cc
  src/core/ext/transport/chttp2/transport/stream_map.cc
  src/core/ext/transport/chttp2/transport/varint.cc
  src/core/ext/transport/chttp2/transport/writing.cc
  src/core/ext/transport/chttp2/alpn/alpn.cc
  src/core/ext/filters/http/client/http_client_filter.cc
  src/core/ext/filters/http/http_filters_plugin.cc
  src/core/ext/filters/http/message_compress/message_compress_filter.cc
  src/core/ext/filters/http/server/http_server_filter.cc
  src/core/lib/http/httpcli_security_connector.cc
  src/core/lib/security/context/security_context.cc
  src/core/lib/security/credentials/composite/composite_credentials.cc
  src/core/lib/security/credentials/credentials.cc
  src/core/lib/security/credentials/credentials_metadata.cc
  src/core/lib/security/credentials/fake/fake_credentials.cc
  src/core/lib/security/credentials/google_default/credentials_generic.cc
  src/core/lib/security/credentials/google_default/google_default_credentials.cc
  src/core/lib/security/credentials/iam/iam_credentials.cc
  src/core/lib/security/credentials/jwt/json_token.cc
  src/core/lib/security/credentials/jwt/jwt_credentials.cc
  src/core/lib/security/credentials/jwt/jwt_verifier.cc
  src/core/lib/security/credentials/oauth2/oauth2_credentials.cc
  src/core/lib/security/credentials/plugin/plugin_credentials.cc
  src/core/lib/security/credentials/ssl/ssl_credentials.cc
  src/core/lib/security/transport/client_auth_filter.cc
  src/core/lib/security/transport/lb_targets_info.cc
  src/core/lib/security/transport/secure_endpoint.cc
  src/core/lib/security/transport/security_connector.cc
  src/core/lib/security/transport/security_handshaker.cc
  src/core/lib/security/transport/server_auth_filter.cc
  src/core/lib/security/transport/tsi_error.cc
  src/core/lib/security/util/json_util.cc
  src/core/lib/surface/init_secure.cc
  src/core/tsi/fake_transport_security.cc
  src/core/tsi/gts_transport_security.cc
  src/core/tsi/ssl_transport_security.cc
  src/core/tsi/transport_security_grpc.cc
  src/core/tsi/transport_security.cc
  src/core/tsi/transport_security_adapter.cc
  src/core/ext/transport/chttp2/server/chttp2_server.cc
  src/core/ext/transport/chttp2/client/secure/secure_channel_create.cc
  src/core/ext/filters/client_channel/backup_poller.cc
  src/core/ext/filters/client_channel/channel_connectivity.cc
  src/core/ext/filters/client_channel/client_channel.cc
  src/core/ext/filters/client_channel/client_channel_factory.cc
  src/core/ext/filters/client_channel/client_channel_plugin.cc
  src/core/ext/filters/client_channel/connector.cc
  src/core/ext/filters/client_channel/http_connect_handshaker.cc
  src/core/ext/filters/client_channel/http_proxy.cc
  src/core/ext/filters/client_channel/lb_policy.cc
  src/core/ext/filters/client_channel/lb_policy_factory.cc
  src/core/ext/filters/client_channel/lb_policy_registry.cc
  src/core/ext/filters/client_channel/parse_address.cc
  src/core/ext/filters/client_channel/proxy_mapper.cc
  src/core/ext/filters/client_channel/proxy_mapper_registry.cc
  src/core/ext/filters/client_channel/resolver.cc
  src/core/ext/filters/client_channel/resolver_factory.cc
  src/core/ext/filters/client_channel/resolver_registry.cc
  src/core/ext/filters/client_channel/retry_throttle.cc
  src/core/ext/filters/client_channel/subchannel.cc
  src/core/ext/filters/client_channel/subchannel_index.cc
  src/core/ext/filters/client_channel/uri_parser.cc
  src/core/ext/filters/deadline/deadline_filter.cc
  src/core/ext/transport/chttp2/client/chttp2_connector.cc
  src/core/ext/transport/chttp2/server/insecure/server_chttp2.cc
  src/core/ext/transport/chttp2/server/insecure/server_chttp2_posix.cc
  src/core/ext/transport/chttp2/client/insecure/channel_create.cc
  src/core/ext/transport/chttp2/client/insecure/channel_create_posix.cc
  src/core/ext/transport/inproc/inproc_plugin.cc
  src/core/ext/transport/inproc/inproc_transport.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/client_load_reporting_filter.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb_channel_secure.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb_client_stats.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/load_balancer_api.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/proto/grpc/lb/v1/load_balancer.pb.c
  third_party/nanopb/pb_common.c
  third_party/nanopb/pb_decode.c
  third_party/nanopb/pb_encode.c
  src/core/ext/filters/client_channel/resolver/fake/fake_resolver.cc
  src/core/ext/filters/client_channel/lb_policy/pick_first/pick_first.cc
  src/core/ext/filters/client_channel/lb_policy/subchannel_list.cc
  src/core/ext/filters/client_channel/lb_policy/round_robin/round_robin.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/dns_resolver_ares.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_ev_driver_posix.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_wrapper.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_wrapper_fallback.cc
  src/core/ext/filters/client_channel/resolver/dns/native/dns_resolver.cc
  src/core/ext/filters/client_channel/resolver/sockaddr/sockaddr_resolver.cc
  src/core/ext/filters/load_reporting/server_load_reporting_filter.cc
  src/core/ext/filters/load_reporting/server_load_reporting_plugin.cc
  src/core/ext/census/grpc_context.cc
  src/core/ext/filters/max_age/max_age_filter.cc
  src/core/ext/filters/message_size/message_size_filter.cc
  src/core/ext/filters/workarounds/workaround_cronet_compression_filter.cc
  src/core/ext/filters/workarounds/workaround_utils.cc
  src/core/plugin_registry/grpc_plugin_registry.cc
)


target_include_directories(grpc
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
)

target_link_libraries(grpc
  ${_gRPC_BASELIB_LIBRARIES}
  ${_gRPC_SSL_LIBRARIES}
  ${_gRPC_ZLIB_LIBRARIES}
  ${_gRPC_CARES_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
  gpr
)


add_library(grpc_unsecure
  src/core/lib/surface/init.cc
  src/core/lib/surface/init_unsecure.cc
  src/core/lib/backoff/backoff.cc
  src/core/lib/channel/channel_args.cc
  src/core/lib/channel/channel_stack.cc
  src/core/lib/channel/channel_stack_builder.cc
  src/core/lib/channel/connected_channel.cc
  src/core/lib/channel/handshaker.cc
  src/core/lib/channel/handshaker_factory.cc
  src/core/lib/channel/handshaker_registry.cc
  src/core/lib/compression/compression.cc
  src/core/lib/compression/message_compress.cc
  src/core/lib/compression/stream_compression.cc
  src/core/lib/compression/stream_compression_gzip.cc
  src/core/lib/compression/stream_compression_identity.cc
  src/core/lib/debug/stats.cc
  src/core/lib/debug/stats_data.cc
  src/core/lib/http/format_request.cc
  src/core/lib/http/httpcli.cc
  src/core/lib/http/parser.cc
  src/core/lib/iomgr/call_combiner.cc
  src/core/lib/iomgr/combiner.cc
  src/core/lib/iomgr/endpoint.cc
  src/core/lib/iomgr/endpoint_pair_posix.cc
  src/core/lib/iomgr/endpoint_pair_uv.cc
  src/core/lib/iomgr/endpoint_pair_windows.cc
  src/core/lib/iomgr/error.cc
  src/core/lib/iomgr/ev_epoll1_linux.cc
  src/core/lib/iomgr/ev_epollex_linux.cc
  src/core/lib/iomgr/ev_epollsig_linux.cc
  src/core/lib/iomgr/ev_poll_posix.cc
  src/core/lib/iomgr/ev_posix.cc
  src/core/lib/iomgr/ev_windows.cc
  src/core/lib/iomgr/exec_ctx.cc
  src/core/lib/iomgr/executor.cc
  src/core/lib/iomgr/fork_posix.cc
  src/core/lib/iomgr/fork_windows.cc
  src/core/lib/iomgr/gethostname_fallback.cc
  src/core/lib/iomgr/gethostname_host_name_max.cc
  src/core/lib/iomgr/gethostname_sysconf.cc
  src/core/lib/iomgr/iocp_windows.cc
  src/core/lib/iomgr/iomgr.cc
  src/core/lib/iomgr/iomgr_posix.cc
  src/core/lib/iomgr/iomgr_uv.cc
  src/core/lib/iomgr/iomgr_windows.cc
  src/core/lib/iomgr/is_epollexclusive_available.cc
  src/core/lib/iomgr/load_file.cc
  src/core/lib/iomgr/lockfree_event.cc
  src/core/lib/iomgr/network_status_tracker.cc
  src/core/lib/iomgr/polling_entity.cc
  src/core/lib/iomgr/pollset_set_uv.cc
  src/core/lib/iomgr/pollset_set_windows.cc
  src/core/lib/iomgr/pollset_uv.cc
  src/core/lib/iomgr/pollset_windows.cc
  src/core/lib/iomgr/resolve_address_posix.cc
  src/core/lib/iomgr/resolve_address_uv.cc
  src/core/lib/iomgr/resolve_address_windows.cc
  src/core/lib/iomgr/resource_quota.cc
  src/core/lib/iomgr/sockaddr_utils.cc
  src/core/lib/iomgr/socket_factory_posix.cc
  src/core/lib/iomgr/socket_mutator.cc
  src/core/lib/iomgr/socket_utils_common_posix.cc
  src/core/lib/iomgr/socket_utils_linux.cc
  src/core/lib/iomgr/socket_utils_posix.cc
  src/core/lib/iomgr/socket_utils_uv.cc
  src/core/lib/iomgr/socket_utils_windows.cc
  src/core/lib/iomgr/socket_windows.cc
  src/core/lib/iomgr/tcp_client_posix.cc
  src/core/lib/iomgr/tcp_client_uv.cc
  src/core/lib/iomgr/tcp_client_windows.cc
  src/core/lib/iomgr/tcp_posix.cc
  src/core/lib/iomgr/tcp_server_posix.cc
  src/core/lib/iomgr/tcp_server_utils_posix_common.cc
  src/core/lib/iomgr/tcp_server_utils_posix_ifaddrs.cc
  src/core/lib/iomgr/tcp_server_utils_posix_noifaddrs.cc
  src/core/lib/iomgr/tcp_server_uv.cc
  src/core/lib/iomgr/tcp_server_windows.cc
  src/core/lib/iomgr/tcp_uv.cc
  src/core/lib/iomgr/tcp_windows.cc
  src/core/lib/iomgr/time_averaged_stats.cc
  src/core/lib/iomgr/timer_generic.cc
  src/core/lib/iomgr/timer_heap.cc
  src/core/lib/iomgr/timer_manager.cc
  src/core/lib/iomgr/timer_uv.cc
  src/core/lib/iomgr/udp_server.cc
  src/core/lib/iomgr/unix_sockets_posix.cc
  src/core/lib/iomgr/unix_sockets_posix_noop.cc
  src/core/lib/iomgr/wakeup_fd_cv.cc
  src/core/lib/iomgr/wakeup_fd_eventfd.cc
  src/core/lib/iomgr/wakeup_fd_nospecial.cc
  src/core/lib/iomgr/wakeup_fd_pipe.cc
  src/core/lib/iomgr/wakeup_fd_posix.cc
  src/core/lib/json/json.cc
  src/core/lib/json/json_reader.cc
  src/core/lib/json/json_string.cc
  src/core/lib/json/json_writer.cc
  src/core/lib/slice/b64.cc
  src/core/lib/slice/percent_encoding.cc
  src/core/lib/slice/slice.cc
  src/core/lib/slice/slice_buffer.cc
  src/core/lib/slice/slice_hash_table.cc
  src/core/lib/slice/slice_intern.cc
  src/core/lib/slice/slice_string_helpers.cc
  src/core/lib/surface/alarm.cc
  src/core/lib/surface/api_trace.cc
  src/core/lib/surface/byte_buffer.cc
  src/core/lib/surface/byte_buffer_reader.cc
  src/core/lib/surface/call.cc
  src/core/lib/surface/call_details.cc
  src/core/lib/surface/call_log_batch.cc
  src/core/lib/surface/channel.cc
  src/core/lib/surface/channel_init.cc
  src/core/lib/surface/channel_ping.cc
  src/core/lib/surface/channel_stack_type.cc
  src/core/lib/surface/completion_queue.cc
  src/core/lib/surface/completion_queue_factory.cc
  src/core/lib/surface/event_string.cc
  src/core/lib/surface/lame_client.cc
  src/core/lib/surface/metadata_array.cc
  src/core/lib/surface/server.cc
  src/core/lib/surface/validate_metadata.cc
  src/core/lib/surface/version.cc
  src/core/lib/transport/bdp_estimator.cc
  src/core/lib/transport/byte_stream.cc
  src/core/lib/transport/connectivity_state.cc
  src/core/lib/transport/error_utils.cc
  src/core/lib/transport/metadata.cc
  src/core/lib/transport/metadata_batch.cc
  src/core/lib/transport/pid_controller.cc
  src/core/lib/transport/service_config.cc
  src/core/lib/transport/static_metadata.cc
  src/core/lib/transport/status_conversion.cc
  src/core/lib/transport/timeout_encoding.cc
  src/core/lib/transport/transport.cc
  src/core/lib/transport/transport_op_string.cc
  src/core/lib/debug/trace.cc
  src/core/ext/transport/chttp2/server/insecure/server_chttp2.cc
  src/core/ext/transport/chttp2/server/insecure/server_chttp2_posix.cc
  src/core/ext/transport/chttp2/transport/bin_decoder.cc
  src/core/ext/transport/chttp2/transport/bin_encoder.cc
  src/core/ext/transport/chttp2/transport/chttp2_plugin.cc
  src/core/ext/transport/chttp2/transport/chttp2_transport.cc
  src/core/ext/transport/chttp2/transport/flow_control.cc
  src/core/ext/transport/chttp2/transport/frame_data.cc
  src/core/ext/transport/chttp2/transport/frame_goaway.cc
  src/core/ext/transport/chttp2/transport/frame_ping.cc
  src/core/ext/transport/chttp2/transport/frame_rst_stream.cc
  src/core/ext/transport/chttp2/transport/frame_settings.cc
  src/core/ext/transport/chttp2/transport/frame_window_update.cc
  src/core/ext/transport/chttp2/transport/hpack_encoder.cc
  src/core/ext/transport/chttp2/transport/hpack_parser.cc
  src/core/ext/transport/chttp2/transport/hpack_table.cc
  src/core/ext/transport/chttp2/transport/http2_settings.cc
  src/core/ext/transport/chttp2/transport/huffsyms.cc
  src/core/ext/transport/chttp2/transport/incoming_metadata.cc
  src/core/ext/transport/chttp2/transport/parsing.cc
  src/core/ext/transport/chttp2/transport/stream_lists.cc
  src/core/ext/transport/chttp2/transport/stream_map.cc
  src/core/ext/transport/chttp2/transport/varint.cc
  src/core/ext/transport/chttp2/transport/writing.cc
  src/core/ext/transport/chttp2/alpn/alpn.cc
  src/core/ext/filters/http/client/http_client_filter.cc
  src/core/ext/filters/http/http_filters_plugin.cc
  src/core/ext/filters/http/message_compress/message_compress_filter.cc
  src/core/ext/filters/http/server/http_server_filter.cc
  src/core/ext/transport/chttp2/server/chttp2_server.cc
  src/core/ext/transport/chttp2/client/insecure/channel_create.cc
  src/core/ext/transport/chttp2/client/insecure/channel_create_posix.cc
  src/core/ext/transport/chttp2/client/chttp2_connector.cc
  src/core/ext/filters/client_channel/backup_poller.cc
  src/core/ext/filters/client_channel/channel_connectivity.cc
  src/core/ext/filters/client_channel/client_channel.cc
  src/core/ext/filters/client_channel/client_channel_factory.cc
  src/core/ext/filters/client_channel/client_channel_plugin.cc
  src/core/ext/filters/client_channel/connector.cc
  src/core/ext/filters/client_channel/http_connect_handshaker.cc
  src/core/ext/filters/client_channel/http_proxy.cc
  src/core/ext/filters/client_channel/lb_policy.cc
  src/core/ext/filters/client_channel/lb_policy_factory.cc
  src/core/ext/filters/client_channel/lb_policy_registry.cc
  src/core/ext/filters/client_channel/parse_address.cc
  src/core/ext/filters/client_channel/proxy_mapper.cc
  src/core/ext/filters/client_channel/proxy_mapper_registry.cc
  src/core/ext/filters/client_channel/resolver.cc
  src/core/ext/filters/client_channel/resolver_factory.cc
  src/core/ext/filters/client_channel/resolver_registry.cc
  src/core/ext/filters/client_channel/retry_throttle.cc
  src/core/ext/filters/client_channel/subchannel.cc
  src/core/ext/filters/client_channel/subchannel_index.cc
  src/core/ext/filters/client_channel/uri_parser.cc
  src/core/ext/filters/deadline/deadline_filter.cc
  src/core/ext/transport/inproc/inproc_plugin.cc
  src/core/ext/transport/inproc/inproc_transport.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/dns_resolver_ares.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_ev_driver_posix.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_wrapper.cc
  src/core/ext/filters/client_channel/resolver/dns/c_ares/grpc_ares_wrapper_fallback.cc
  src/core/ext/filters/client_channel/resolver/dns/native/dns_resolver.cc
  src/core/ext/filters/client_channel/resolver/sockaddr/sockaddr_resolver.cc
  src/core/ext/filters/client_channel/resolver/fake/fake_resolver.cc
  src/core/ext/filters/load_reporting/server_load_reporting_filter.cc
  src/core/ext/filters/load_reporting/server_load_reporting_plugin.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/client_load_reporting_filter.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb_channel.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/grpclb_client_stats.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/load_balancer_api.cc
  src/core/ext/filters/client_channel/lb_policy/grpclb/proto/grpc/lb/v1/load_balancer.pb.c
  third_party/nanopb/pb_common.c
  third_party/nanopb/pb_decode.c
  third_party/nanopb/pb_encode.c
  src/core/ext/filters/client_channel/lb_policy/pick_first/pick_first.cc
  src/core/ext/filters/client_channel/lb_policy/subchannel_list.cc
  src/core/ext/filters/client_channel/lb_policy/round_robin/round_robin.cc
  src/core/ext/census/grpc_context.cc
  src/core/ext/filters/max_age/max_age_filter.cc
  src/core/ext/filters/message_size/message_size_filter.cc
  src/core/ext/filters/workarounds/workaround_cronet_compression_filter.cc
  src/core/ext/filters/workarounds/workaround_utils.cc
  src/core/plugin_registry/grpc_unsecure_plugin_registry.cc
)


target_include_directories(grpc_unsecure
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
)

target_link_libraries(grpc_unsecure
  ${_gRPC_BASELIB_LIBRARIES}
  ${_gRPC_ZLIB_LIBRARIES}
  ${_gRPC_CARES_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
  gpr
)

set_target_properties(grpc_unsecure PROPERTIES SOVERSION 6)

add_library(grpc++
  src/cpp/client/insecure_credentials.cc
  src/cpp/client/secure_credentials.cc
  src/cpp/common/auth_property_iterator.cc
  src/cpp/common/secure_auth_context.cc
  src/cpp/common/secure_channel_arguments.cc
  src/cpp/common/secure_create_auth_context.cc
  src/cpp/server/insecure_server_credentials.cc
  src/cpp/server/secure_server_credentials.cc
  src/cpp/client/channel_cc.cc
  src/cpp/client/client_context.cc
  src/cpp/client/create_channel.cc
  src/cpp/client/create_channel_internal.cc
  src/cpp/client/create_channel_posix.cc
  src/cpp/client/credentials_cc.cc
  src/cpp/client/generic_stub.cc
  src/cpp/common/channel_arguments.cc
  src/cpp/common/channel_filter.cc
  src/cpp/common/completion_queue_cc.cc
  src/cpp/common/core_codegen.cc
  src/cpp/common/resource_quota_cc.cc
  src/cpp/common/rpc_method.cc
  src/cpp/common/version_cc.cc
  src/cpp/server/async_generic_service.cc
  src/cpp/server/channel_argument_option.cc
  src/cpp/server/create_default_thread_pool.cc
  src/cpp/server/dynamic_thread_pool.cc
  src/cpp/server/health/default_health_check_service.cc
  src/cpp/server/health/health.pb.c
  src/cpp/server/health/health_check_service.cc
  src/cpp/server/health/health_check_service_server_builder_option.cc
  src/cpp/server/server_builder.cc
  src/cpp/server/server_cc.cc
  src/cpp/server/server_context.cc
  src/cpp/server/server_credentials.cc
  src/cpp/server/server_posix.cc
  src/cpp/thread_manager/thread_manager.cc
  src/cpp/util/byte_buffer_cc.cc
  src/cpp/util/slice_cc.cc
  src/cpp/util/status.cc
  src/cpp/util/string_ref.cc
  src/cpp/util/time_cc.cc
  src/cpp/codegen/codegen_init.cc
)


target_include_directories(grpc++
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTO_GENS_DIR}
)
target_link_libraries(grpc++
  ${_gRPC_BASELIB_LIBRARIES}
  ${_gRPC_SSL_LIBRARIES}
  ${_gRPC_PROTOBUF_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
  grpc
  gpr
)

add_library(grpc++_unsecure
  src/cpp/client/insecure_credentials.cc
  src/cpp/common/insecure_create_auth_context.cc
  src/cpp/server/insecure_server_credentials.cc
  src/cpp/client/channel_cc.cc
  src/cpp/client/client_context.cc
  src/cpp/client/create_channel.cc
  src/cpp/client/create_channel_internal.cc
  src/cpp/client/create_channel_posix.cc
  src/cpp/client/credentials_cc.cc
  src/cpp/client/generic_stub.cc
  src/cpp/common/channel_arguments.cc
  src/cpp/common/channel_filter.cc
  src/cpp/common/completion_queue_cc.cc
  src/cpp/common/core_codegen.cc
  src/cpp/common/resource_quota_cc.cc
  src/cpp/common/rpc_method.cc
  src/cpp/common/version_cc.cc
  src/cpp/server/async_generic_service.cc
  src/cpp/server/channel_argument_option.cc
  src/cpp/server/create_default_thread_pool.cc
  src/cpp/server/dynamic_thread_pool.cc
  src/cpp/server/health/default_health_check_service.cc
  src/cpp/server/health/health.pb.c
  src/cpp/server/health/health_check_service.cc
  src/cpp/server/health/health_check_service_server_builder_option.cc
  src/cpp/server/server_builder.cc
  src/cpp/server/server_cc.cc
  src/cpp/server/server_context.cc
  src/cpp/server/server_credentials.cc
  src/cpp/server/server_posix.cc
  src/cpp/thread_manager/thread_manager.cc
  src/cpp/util/byte_buffer_cc.cc
  src/cpp/util/slice_cc.cc
  src/cpp/util/status.cc
  src/cpp/util/string_ref.cc
  src/cpp/util/time_cc.cc
  src/cpp/codegen/codegen_init.cc
)


target_include_directories(grpc++_unsecure
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTO_GENS_DIR}
)
target_link_libraries(grpc++_unsecure
  ${_gRPC_BASELIB_LIBRARIES}
  ${_gRPC_PROTOBUF_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
  gpr
  grpc_unsecure
)

set_target_properties(grpc++_unsecure PROPERTIES SOVERSION 1)


add_library(grpc_plugin_support
  src/compiler/cpp_generator.cc
  src/compiler/csharp_generator.cc
  src/compiler/node_generator.cc
  src/compiler/objective_c_generator.cc
  src/compiler/php_generator.cc
  src/compiler/python_generator.cc
  src/compiler/ruby_generator.cc
)


target_include_directories(grpc_plugin_support
  PUBLIC $<INSTALL_INTERFACE:${gRPC_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTO_GENS_DIR}
)
target_link_libraries(grpc_plugin_support
  ${_gRPC_PROTOBUF_PROTOC_LIBRARIES}
  ${_gRPC_PROTOBUF_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
)

add_executable(grpc_cpp_plugin
  src/compiler/cpp_plugin.cc
)
target_include_directories(grpc_cpp_plugin
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${_gRPC_SSL_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTOBUF_INCLUDE_DIR}
  PRIVATE ${_gRPC_ZLIB_INCLUDE_DIR}
  PRIVATE ${_gRPC_BENCHMARK_INCLUDE_DIR}
  PRIVATE ${_gRPC_CARES_INCLUDE_DIR}
  PRIVATE ${_gRPC_GFLAGS_INCLUDE_DIR}
  PRIVATE ${_gRPC_PROTO_GENS_DIR}
)

target_link_libraries(grpc_cpp_plugin
  ${_gRPC_PROTOBUF_PROTOC_LIBRARIES}
  ${_gRPC_PROTOBUF_LIBRARIES}
  ${_gRPC_ALLTARGETS_LIBRARIES}
  grpc_plugin_support
)

