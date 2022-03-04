#ifndef CUPOLLDATAUPDPOLICY_ENUM_H
#define CUPOLLDATAUPDPOLICY_ENUM_H

/*!
* \par Update policy
* \since 1.4.0
*
* options can be used to provide update hints to the reader.
* In case of *polled* sources, one of the values of the enum class can be specified in order to
* avoid updates when data does not change. In case of rich applications, this improves efficiency.
*
* The available *hints* are as follows:
* - UpdateAlways: the data listener shall always be updated
* - OnUnchangedTimestampOnly: the data listener shall always be updated with a
*   minimal CuData with *timestamp_us* and *timestamp_ms* if data does not change
* - OnUnchangedNothing: the data listener shall *not* be updated as long as
*   read data remains unchanged.
*
* \note
* Update policy hint applies to *polled sources* only.
*
* Data is considered to have changed if:
* - the value (read or Tango set point) has changed from to the previous reading
* - an error condition changes
* - the attribute *data quality* changes
*
* @see CuPollingService
*/
enum CuDataUpdatePolicy {
    PollUpdateAlways = 0x1, OnPollUnchangedTimestampOnly = 0x2, OnPollUnchangedNoUpdate = 0x4,
    SkipFirstReadUpdate = 0x8, EndOptions = 0x20
};

#endif // DATAUPDPOLICY_ENUM_H
