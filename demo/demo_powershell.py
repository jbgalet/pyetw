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

    provider = etw.Provider("{A0C1853B-5C40-4B15-8766-3CF1C58F985A}", callback=prov_callback)
    provider.any(0xf0010000000003ff);

    trace.enable(provider)

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
