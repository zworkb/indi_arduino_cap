# INDI API Migration Dokumentation

## Übersicht

Dieses Dokument beschreibt die Änderungen, die vorgenommen wurden, um den Arduino Cap Treiber mit der neuesten INDI API kompatibel zu machen.

## Datum der Migration

2026-01-04

## Hauptänderungen

### 1. Interface-Initialisierung

**Datei:** `arduino_cap/arduino_cap.cpp`, Zeilen 105-107

#### Vorher:
```cpp
initDustCapProperties(getDeviceName(), MAIN_CONTROL_TAB);
initLightBoxProperties(getDeviceName(), MAIN_CONTROL_TAB);
```

#### Nachher:
```cpp
DustCapInterface::initProperties(MAIN_CONTROL_TAB);    // statt initDustCapProperties()
LightBoxInterface::initProperties(MAIN_CONTROL_TAB, 0);   // statt initLightBoxProperties()
DefaultDevice::initProperties();
```

**Grund:** Die neuen Interface-Klassen verwenden statische Methoden ohne `getDeviceName()` Parameter. `LightBoxInterface::initProperties()` benötigt zusätzlich einen zweiten Parameter (Capability-Flags).

---

### 2. Property-Zugriffsmethoden (ParkCapSP)

**Dateien:** `arduino_cap/arduino_cap.cpp`
- Zeilen 178-195 (SetupParams)
- Zeilen 450-453 (ParkCap)
- Zeilen 474-477 (UnParkCap)
- Zeilen 496-502 (SetOKParkStatus)

#### Vorher:
```cpp
IUResetSwitch(&ParkCapSP);
ParkCapS[0].s = ISS_ON;
ParkCapSP.s = IPS_OK;
IDSetSwitch(&ParkCapSP, NULL);
```

#### Nachher:
```cpp
ParkCapSP.reset();
ParkCapSP[0].setState(ISS_ON);
ParkCapSP.setState(IPS_OK);
ParkCapSP.apply();
```

**Grund:** Die neue INDI API verwendet Property-Wrapper-Klassen (`INDI::PropertySwitch`) mit objektorientierten Methoden anstelle direkter Struktur-Zugriffe.

---

### 3. Interface Process-Methoden

**Datei:** `arduino_cap/arduino_cap.cpp`

#### ISNewNumber (Zeile 302):
```cpp
// Vorher:
if (processLightBoxNumber(dev, name, values, names, n))

// Nachher:
if (LightBoxInterface::processNumber(dev, name, values, names, n))
```

#### ISNewText (Zeile 367):
```cpp
// Vorher:
if (processLightBoxText(dev, name, texts, names, n))

// Nachher:
if (LightBoxInterface::processText(dev, name, texts, names, n))
```

#### ISNewSwitch (Zeilen 385, 388):
```cpp
// Vorher:
if (processLightBoxSwitch(dev, name, states, names, n))
    return true;
if (processDustCapSwitch(dev, name, states, names, n))
    return true;

// Nachher:
if (LightBoxInterface::processSwitch(dev, name, states, names, n))
    return true;
if (DustCapInterface::processSwitch(dev, name, states, names, n))
    return true;
```

**Grund:** Die Interface-Methoden sind jetzt statische Methoden der Interface-Klassen.

---

### 4. Property-Namen-Zugriff

**Datei:** `arduino_cap/arduino_cap.cpp`
- Zeilen 238-239 (updateProperties - deleteProperty)
- Zeilen 404 (ISNewSwitch - deleteProperty)
- Zeilen 521, 633 (Move, Move2 - ISNewSwitch)

#### Vorher:
```cpp
deleteProperty(LightSP.name);
deleteProperty(ParkCapSP.name);
ISNewSwitch(getDeviceName(), LightSP.name, states, names, 2);
```

#### Nachher:
```cpp
deleteProperty(LightSP.getName());
deleteProperty(ParkCapSP.getName());
ISNewSwitch(getDeviceName(), LightSP.getName(), states, names, 2);
```

**Grund:** Property-Namen werden jetzt über die Methode `getName()` abgerufen statt über direkten Zugriff auf das `name`-Feld.

---

### 5. Property-Definition

**Datei:** `arduino_cap/arduino_cap.cpp`
- Zeile 211 (updateProperties)
- Zeilen 402, 781 (ISNewSwitch, TimerHit)

#### Vorher:
```cpp
defineProperty(&ParkCapSP);
defineProperty(&LightSP);
```

#### Nachher:
```cpp
defineProperty(ParkCapSP);
defineProperty(LightSP);
```

**Grund:** Die neue API akzeptiert Property-Wrapper direkt ohne `&`-Operator, da der `&`-Operator in der neuen Property-Klasse als `protected` deklariert ist.

---

## Betroffene Klassen und Interfaces

1. **INDI::DustCapInterface**
   - `initProperties()` - neue Signatur
   - `processSwitch()` - statische Methode

2. **INDI::LightBoxInterface**
   - `initProperties()` - neue Signatur mit zusätzlichem Parameter
   - `processNumber()` - statische Methode
   - `processText()` - statische Methode
   - `processSwitch()` - statische Methode

3. **INDI::PropertySwitch** (neue Wrapper-Klasse)
   - `reset()` - ersetzt `IUResetSwitch()`
   - `setState()` - ersetzt direkte Zugriffe auf `.s` und `[i].s`
   - `apply()` - ersetzt `IDSetSwitch()`
   - `getName()` - ersetzt direkten Zugriff auf `.name`

---

## Build-Ergebnis

Nach diesen Änderungen kompiliert das Projekt erfolgreich:
```
[ 33%] Building CXX object CMakeFiles/indi_arduino_cap.dir/arduino_cap/arduino_cap.cpp.o
[ 66%] Building CXX object CMakeFiles/indi_arduino_cap.dir/arduino_cap/parkdata.cpp.o
[100%] Linking CXX executable indi_arduino_cap
[100%] Built target indi_arduino_cap
```

---

## Kompatibilität

Diese Änderungen machen den Treiber kompatibel mit:
- INDI Library 2.x (neueste Version)
- Die Änderungen sind nicht rückwärtskompatibel mit älteren INDI-Versionen

---

## Weitere Hinweise

### Nicht geänderte Bereiche

Die folgenden alten C-Strukturen werden weiterhin verwendet, da sie noch keine modernisierten Wrapper haben:
- `INumberVectorProperty` und `INumber` (z.B. `MoveSteppNP`, `ServoIDNP`)
- `ISwitchVectorProperty` und `ISwitch` (z.B. `LightTypeSP`, `HasSecondServoSNP`)
- `ITextVectorProperty` und `IText` (z.B. `DevicePathTP`)

Diese verwenden weiterhin die alten Funktionen wie:
- `IUFillNumber()`, `IUFillSwitch()`, `IUFillText()`
- `IUUpdateNumber()`, `IUUpdateSwitch()`, `IUUpdateText()`
- `IDSetNumber()`, `IDSetSwitch()`, `IDSetText()`

### Zukünftige Migration

Falls INDI in Zukunft auch für diese Property-Typen modernisierte Wrapper einführt, müssten diese ebenfalls migriert werden.

---

## Referenzen

- INDI Library GitHub: https://github.com/indilib/indi
- INDI API Dokumentation: https://www.indilib.org/api/