#ifndef QSIMPLESIGNALAGGREGATOR_H
#define QSIMPLESIGNALAGGREGATOR_H

#include <QHash>

// #include <QDebug>

#include "qsignalaggregator.h"

/**
 * @class QSimpleSignalAggregator
 * @brief Simple implementation of a QSignalAggregator.
 *
 * This simple aggregator is as rudimentary as an aggregator gets.
 *
 * Here is client code with a simple use-case:
 *
 * \code{.cpp}
 * QSimpleSignalAggregator *aggregator = new QSimpleSignalAggregator(this);
 * QObject::connect(aggregator, SIGNAL(done()), this, SLOT(allSignalsInvoked()));
 * aggregator->aggregate(foo, SIGNAL(done()));
 * aggregator->aggregate(bar, SIGNAL(done()));
 * \endcode
 *
 * Here, \c aggregator will emit \c done() only after both \c
 * foo::done() and \c bar::done() have been emitted.
 *
 * To turn this one-shot behavior into a continuous process, chain the
 * \c done() signal into the same object's \c reset() slot.
 *
 * \code{.cpp}
 * QObject::connect(aggregator, SIGNAL(done()), aggregator, SLOT(reset()));
 * \endcode
 *
 * @see QSignalAggregator
 */
class QSimpleSignalAggregator : public QSignalAggregator {
    Q_OBJECT

public:
    /**
     * @brief Create a new signal aggregator with specified parent.
     *
     * @param parent Parent QObject of the new aggregator.
     */
    QSimpleSignalAggregator(QObject *parent);

    // TODO: support initializer list of sender & signal pairs

    /**
     * @brief Destructor
     */
    ~QSimpleSignalAggregator() { }

    /**
     * @copydic QSignalAggregator::aggregate(const QObject *sender,
     * const char *signal, const int occurrences = 1)
     */
    auto aggregate(const QObject *sender,
                   const char *signal,
                   const int occurrences = 1) -> void override;

    /**
     * @copydoc QSignalAggregator::gate(QObject *sender, const char *signal)
     */
    auto gate(QObject *sender,
              const char *signal) -> void override;

    // auto printStatus() -> void {
    //     qDebug() << "Here goes the current status so far";
    //     QHash<const QObject *, QHash<const QString, int> >::iterator i;
    //     for (i = lookingFor.begin(); i != lookingFor.end(); ++i)
    //         qDebug() << i.key() << ": " << i.value() << endl;
    // }

public slots:
    /**
     * @brief Disregard all previously-witnessed signals and begin
     * monitoring for all registered signals again.
     */
    void reset();

private:
    /**
     * @brief Check the status of monitored signals, and emit the
     * gated signal if appropriate.
     */
    auto checkAggregationStatus() -> void;

    /**
     * @brief Emit the gated signal.
     */
    auto emitGatedSignal() -> void;

    /**
     * @brief Convert the <tt>const char *</tt> output of the \c
     * SIGNAL() macro into a \c QString identifying a method name
     * suitable for use in \c QMetaObject::invokeMethod.
     *
     * \b Note: currently does not support methods with signatures
     * (e.g. only methods ending in \c () are supported).
     *
     * \todo Support emitting a signal with parameters
     * http://stackoverflow.com/questions/7721923/calling-qmetaobjectinvokemethod-with-variable-amount-of-parameters
     *
     * @param signal Signal to extract method name from.
     */
    auto convertSignalIntoInvokableMethod(const char *signal) -> QString;

private slots:
    /**
     * @brief Handle an event from one of the monitored signals and
     * update the internal tracking mechanism.
     */
    void captureSignal();

private:
    /**
     * @var lookingFor
     *
     * @brief Map of monitored signals, index by the objects that will
     * emit them.  When signals are witnessed, they are removed from
     * this object until none remain.  At this point, the aggregated
     * signal will be fired.
     */
    QHash<const QObject *, QHash<const QString, int> > lookingFor;

    /**
     * @var monitor
     *
     * @brief Map of monitored signals, index by the objects that will
     * emit them.  Only grows, never shrinks, used during \c reset().
     */
    QHash<const QObject *, QHash<const QString, int> > monitor;

    /**
     * @var triggerObject
     *
     * @brief Object to emit \c triggerSignal from upon aggregation of
     * all monitored signals.
     */
    QObject *triggerObject;

    /**
     * @var triggerSignal
     *
     * @brief Signal to trigger upon aggregation of all monitored
     * signals.
     */
    QString triggerSignal;
};

#endif // QSIMPLESIGNALAGGREGATOR_H
