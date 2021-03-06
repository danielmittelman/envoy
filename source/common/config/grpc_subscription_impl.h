#pragma once

#include "envoy/config/grpc_mux.h"
#include "envoy/config/subscription.h"
#include "envoy/event/dispatcher.h"

#include "common/common/logger.h"

namespace Envoy {
namespace Config {

/**
 * Adapter from typed Subscription to untyped GrpcMux. Also handles per-xDS API stats/logging.
 */
class GrpcSubscriptionImpl : public Subscription,
                             GrpcMuxCallbacks,
                             Logger::Loggable<Logger::Id::config> {
public:
  GrpcSubscriptionImpl(GrpcMuxSharedPtr grpc_mux, SubscriptionCallbacks& callbacks,
                       SubscriptionStats stats, absl::string_view type_url,
                       Event::Dispatcher& dispatcher, std::chrono::milliseconds init_fetch_timeout,
                       bool is_aggregated);

  // Config::Subscription
  void start(const std::set<std::string>& resource_names) override;
  void updateResourceInterest(const std::set<std::string>& update_to_these_names) override;

  // Config::GrpcMuxCallbacks
  void onConfigUpdate(const Protobuf::RepeatedPtrField<ProtobufWkt::Any>& resources,
                      const std::string& version_info) override;
  void onConfigUpdateFailed(Envoy::Config::ConfigUpdateFailureReason reason,
                            const EnvoyException* e) override;
  std::string resourceName(const ProtobufWkt::Any& resource) override;

  GrpcMuxSharedPtr grpcMux() { return grpc_mux_; }

private:
  void disableInitFetchTimeoutTimer();

  GrpcMuxSharedPtr grpc_mux_;
  SubscriptionCallbacks& callbacks_;
  SubscriptionStats stats_;
  const std::string type_url_;
  GrpcMuxWatchPtr watch_{};
  Event::Dispatcher& dispatcher_;
  std::chrono::milliseconds init_fetch_timeout_;
  Event::TimerPtr init_fetch_timeout_timer_;
  const bool is_aggregated_;
};

} // namespace Config
} // namespace Envoy
