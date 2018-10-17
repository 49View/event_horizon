#pragma once

#include <functional>
#include <memory>
#include <atomic>

#include "sh/calculator_context.h"
#include "core/task_utils.h"

class SHController {
public:
    SHController();

    void calculateAsync( std::vector<std::shared_ptr<HierGeom>>& sceneGeoms,
                         std::function<void( void )> onCompletionFunc );

    void setOptions( CalculatorOption calcOptions );
    void cancel();
    void waitForFinish();
private:

    CalculatorOption options;
    std::shared_ptr<std::future<void>> calculationTask;
    CancellationTokenSource cancelationTokenSource;
    std::atomic_bool running;
};
