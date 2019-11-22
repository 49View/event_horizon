#include "sh_controller.h"
#include "sh/sh_calculator.h"
#include "ray_tracing/geometry_offset.h"

SHController::SHController() {
    running.store( false );
}

void foo() {

}

void SHController::calculateAsync( std::vector<GeomSP>& sceneGeoms,
                                   std::function<void( void )> onCompletionFunc ) {
    if ( running.load()) return;
    running.store( true );
    LOGR( "Start calculating spherical harmonics" );

    SHCalculator calc;
    cancelationTokenSource = CancellationTokenSource();

    auto onProgress = []( std::string stage, size_t progress, size_t total ) {
        LOGR( "%s: %i of %i", stage.c_str(), progress, total );
    };
    auto onCompletion = [onCompletionFunc, this]() {
        LOGR( "End spherical harmonics" );
        running.store( false );
        onCompletionFunc();
    };

    auto fc = std::bind( &SHCalculator::calculate, calc, sceneGeoms, options,
                         cancelationTokenSource.getToken(), onProgress, onCompletion );

//    calc.calculate( sceneGeoms, options, cancelationTokenSource.getToken(), onProgress, onCompletion );
    calculationTask = std::make_shared<std::future<void>>( std::async( std::launch::async, fc ));

    LOGR( "SH calculation is running" );
}

void SHController::setOptions( CalculatorOption calcOptions ) {
    options = calcOptions;
}

void SHController::cancel() {
    cancelationTokenSource.cancel();
    //running.store( false );
}

void SHController::waitForFinish() {
    if ( running.load())
        calculationTask->wait();
}
