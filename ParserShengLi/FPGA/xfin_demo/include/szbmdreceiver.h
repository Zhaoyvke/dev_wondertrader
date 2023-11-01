#ifndef MARKETDATARECEIVER_H
#define MARKETDATARECEIVER_H

#include "mdreceiver_global.h"
#include "szbdatadef.h"

class SZBMDReceiverPrivate;
class MARKETDATARECEIVER_EXPORT SZBMDReceiver
{
public:
    explicit SZBMDReceiver();
    ~SZBMDReceiver();

    /**
     * @brief init
     * Initialize receiver with param.
     *
     * @param param
     * Parameter to initialize receiver.
     *
     * @return bool
     * Return true if initialized, otherwise return false.
     */
    bool init(const SZBRecvParam &param);

    /**
     * @brief init
     * Overload function. Initialize receiver with config file and callback handler.
     *
     * @param configFile
     * Config file to initialize receiver.
     *
     * @param handler
     * Callback handler to receive data.
     *
     * @return bool
     * Return true if initialized, otherwise return false.
     */
    bool init(const char *configFile, SZBMsgHandler *handler);

    /**
     * @brief start
     * Start receiver.
     *
     * @return bool
     * Return true if started, otherwise return false.
     */
    bool start();

    /**
     * @brief stop
     * Stop receiver.
     */
    void stop();

private:
    SZBMDReceiverPrivate * const d_ptr;
};

#endif // MARKETDATARECEIVER_H
