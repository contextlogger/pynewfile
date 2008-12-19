import pynewfile

# This does not work even though Gallery is launched.
#gallery_uid = 0x101f8599
#print repr(pynewfile.take_photo(gallery_uid))

import sysinfo

osv = sysinfo.os_version()
if osv == (2,0,1528): # N95
    uid = 0x101ffa86
elif osv == (2,0,1071): # E61 (has no camera)
    uid = 0x101f857a # works on N91 apparently
else:
    raise "do not know what UID to use on this phone"

print repr(pynewfile.take_photo(uid))
