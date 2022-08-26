#ifndef QSIGNALAGGREGATOR_H
#define QSIGNALAGGREGATOR_H

#include <QObject>

/**
 * @interface QSignalAggregator
 * @brief Interface for an object acting as a Qt Signal aggregator.
 *
 * A signal aggregator acts as a
 * <a href="https://en.wikipedia.org/wiki/Barrier_(computer_science)">barrier</a>,
 * listening for multiple signals before emitting a gated signal.
 *
 * @see QSimpleSignalAggregator
 */
class QSignalAggregator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Invokes the QObject constructor with specified parent.
     */
    explicit QSignalAggregator(QObject *parent) : QObject(parent) { }

    /**
     * @brief Destructor.
     */
    virtual ~QSignalAggregator() { }

    /**
     * @brief Add the specified signal to the list of aggregated
     * (monitored) signals.  This object will emit \c SIGNAL(done())
     * only when all signals specified this way have been recorded.
     *
     * @param sender The object to monitor for emission of \c signal.
     * @param signal The signal to aggregate.
     *
     * @param occurrences Number of times to aggregate the specified
     * signal (defaults to 1).
     */
    virtual auto aggregate(const QObject *sender,
                           const char *signal,
                           const int occurrences) -> void = 0;

    /**
     * @brief Gate the specified signal instead of the \c
     * QSimpleSignalAggregator default (\c SIGNAL(done())).
     *
     * @param sender The object used to emit \c signal when sufficient
     * conditions are met.
     *
     * \b Note: Only signals without parameters may be invoked at this
     * time.
     *
     * @param signal The signal to emit when all monitored signals
     * have been aggregated
     */
    virtual auto gate(QObject *sender,
                      const char *signal) -> void = 0;

signals:
    /**
     * @brief Signal emitted upon successful aggregation
     * (interception) of all monitored signals.
     */
    void done();
};

#endif // QSIGNALAGGREGATOR_H
