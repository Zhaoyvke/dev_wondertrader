#ifndef SSEMDRECEIVER_H
#define SSEMDRECEIVER_H

#include "mdreceiver_global.h"
#include "ssedatadef.h"

class SSEMDReceiverPrivate;
class MARKETDATARECEIVER_EXPORT SSEMDReceiver
{
public:
    explicit SSEMDReceiver();
    ~SSEMDReceiver();

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
    bool init(const SSERecvParam &param);

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
    bool init(const char *configFile, SSEMsgHandler *handler);

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
    SSEMDReceiverPrivate * const d_ptr;
};

#endif // SSEMDRECEIVER_H
