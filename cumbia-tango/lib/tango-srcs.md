# Tango source options and syntax

### Read an attribute

- test/device/1/double_scalar

### "Read a command"

Commands providing an output arg can be "read". The syntax
requires a *->* separator between the device name and the 
command name. Command arguments shall be enclosed within
parentheses:

- test/device/1->DevLong(10)

- srv-tango-ctrl-01:20000/test/device/1->DevLong(10)

- tango://srv-tango-ctrl-01:20000/test/device/1->DevLong(10)

Please note that in the last example, the *tango://* prefix
is used to explicitly pick the *Tango* engine among those
registered in the application.

### Domain search

Provide a keyword with a final '*'  to search Tango domains

- tes* 

- srv-tango-ctrl-01:20000/tes*

#### Example using cumbia read

> cumbia read -n "tes*"
 
```  * tes*: [14:22:41+0.760138] [string,3] { test,test2,test64bit }
```

(-n tells *cumbia read* to use the *native* Tango engine)

### Family search

- test/dev* 

### Member search 

- test/device/*

- test/device/1*

#### Example with cumbia read

> cumbia read -n "test/device/1*"

```
* test/device/1*: [14:24:16+0.645212] [string,11] { 1,10,11,12,13,14,15,16,17,18,19 }
```

### Get the list of attributes for a given device

- test/device/1/

#### Example with cumbia read

> cumbia read -n "test/device/1/" 
```
* test/device/1/: [14:24:48+0.0695989] [string,62] { ampli,boolean_scalar,double_scalar,double_scalar_rww,double_scalar_w,float_scalar,long64_scalar,long_scalar,long_scalar_rww,long_scalar_w,no_value,short_scalar,short_scalar_ro,short_scalar_rww,short_scalar_w,string_scalar,throw_exception,uchar_scalar,ulong64_scalar,ushort_scalar,ulong_scalar,boolean_spectrum,boolean_spectrum_ro,double_spectrum,double_spectrum_ro,float_spectrum,float_spectrum_ro,long64_spectrum_ro,long_spectrum,long_spectrum_ro,short_spectrum,short_spectrum_ro,string_spectrum,string_spectrum_ro,uchar_spectrum,uchar_spectrum_ro,ulong64_spectrum_ro,ulong_spectrum_ro,ushort_spectrum,ushort_spectrum_ro,wave,boolean_image,boolean_image_ro,double_image,double_image_ro,float_image,float_image_ro,long64_image_ro,long_image,long_image_ro,short_image,short_image_ro,string_image,string_image_ro,uchar_image,uchar_image_ro,ulong64_image_ro,ulong_image_ro,ushort_image,ushort_image_ro,State,Status }
```


#### Example with QuWatcher 

```
QuWatcher *w = new QuWatcher(...);
w->setSource("test/device/1/");
```

### Tango database free property

To read a *free Tango database property*, we shall use the '#'
special character: #Name#PropertyName

- #Astor#HostDialogPreferredSize

- #AlarmHandler#DeviceList

### Tango device properties

- tango://srv-tango-ctrl-01:20000/test/device/1(description, helperApplication,values)

### Single attribute property

- test/device/1/double_scalar(max_value)

### Multiple attribute properties

- test/device/1/double_scalar(max_value,min_value)

#### QuWatcher example

```cpp
QuWatcher *w = new QuWatcher(this, cu_pool, m_ctrl_factory_pool);
w->attach(this, SLOT(onUpdate(const CuData&)));
w->setSource("test/device/1/double_scalar(max_value,min_value)");
```

We would receive a CuData like this:
```cpp
void MyApp::onUpdate(const CuData& da) {
    printf("data-> %s\n", datos(da));
}
```

```
{ 
    CuData { 
        ["timestamp_us" -> 1.68994e+09], 
        ["msg" -> operation "SrcDbAProp" successful], 
        ["keys" -> format,max_alarm,max_value,min_alarm,min_value], 
        ["period" -> 1000], 
        ["min_value" -> -255], 
        ["op" -> SrcDbAProp], 
        ["value" -> -255], 
        ["pattern" -> ], 
        ["single-shot" -> true], 
        ["min_alarm" -> -120], 
        ["type" -> property], 
        ["err" -> false], 
        ["point" -> double_scalar], 
        ["src" -> tango://srv-tango-ctrl-01:20000/test/device/1/double_scalar(min_value,max_value,min_alarm,max_alarm,format)], 
        ["device" -> tango://srv-tango-ctrl-01:20000/test/device/1], 
        ["max_alarm" -> 225], 
        ["timestamp_ms" -> 1689943451795], 
        ["format" -> %4.1f], 
        ["max_value" -> 255] 
    } (size: 19 isEmpty: 0) 
}
```

Please note the *keys* key with the list of requested properties and then
each property as a key with value the corresponding value, for example
*max_alarm* --> *255*

### List all properties of a given class

- cumbia read -n "TangoTest(*)" -l debug

The *-l debug* switch shall produce the following output (prints the
full contents of the *CuData* received):

```
{ 
CuData 
    {   ["timestamp_us" -> 1.68994e+09], 
        ["msg" -> operation "SrcDbClassProps" successful], 
        ["class" -> TangoTest], 
        ["err" -> false], 
        ["type" -> property], 
        ["value" -> Csoglia,cvs_location,cvs_tag,Description,doc_url,InheritedFrom,ProjectTitle], 
        ["pattern" -> TangoTest(*)], 
        ["op" -> SrcDbClassProps], 
        ["single-shot" -> true], 
        ["period" -> 1000], 
        ["timestamp_ms" -> 1689943862571], 
        ["src" -> TangoTest(*)] 
    } (size: 12 isEmpty: 0) 
}

```

### Fetch class properties

- TangoTest(ProjectTitle,Description)

```
{ 
    CuData 
    { 
        ["timestamp_ms" -> 1689944015492],
        ["src" -> TangoTest(ProjectTitle,Description)],
        ["period" -> 1000], 
        ["keys" -> ProjectTitle,Description], 
        ["single-shot" -> true], 
        ["op" -> SrcDbClassProp], 
        ["pattern" -> ], 
        ["value" -> TANGO Device Server for testing generic clients],
        ["type" -> property], 
        ["class" -> TangoTest], 
        ["timestamp_us" -> 1.68994e+09], 
        ["Description" -> A device to test generic clients. It offers a "echo" like command for,each TANGO data type (i.e. each command returns an exact copy of <argin>).], 
        ["msg" -> operation "SrcDbClassProp" successful], 
        ["err" -> false], 
        ["ProjectTitle" -> TANGO Device Server for testing generic clients] 
    } (size: 15 isEmpty: 0) 
}
```
