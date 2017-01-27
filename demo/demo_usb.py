import pprint
import threading
import time

import etw


def thread_func(trace):
    trace.start()
    print("ThreadEND")


def prov_callback(event_name: str, event_id: int, provider_name: str, properties: dict) -> None:
    print(event_name, event_id, provider_name)
    pprint.pprint(properties)
    return None


def main():
    print("pyETW")
    print(dir(etw))
    trace = etw.UserTrace("PyTrace")

    usb_prov = etw.Provider("{ac52ad17-cc01-4f85-8df5-4dce4333c99b}", callback=prov_callback)

    trace.enable(usb_prov)

    t = threading.Thread(target=thread_func, args=(trace,))
    t.start()

    try:
        print("Sleep 30 sec")
        time.sleep(30)
    except:
        print("Interrupted!")
    finally:
        print("Stop Trace")
        trace.stop()
        print("Join Thread")
        t.join()
    print("OK")


if __name__ == '__main__':
    main()
