# PERCUSSION ANALIZER (VST3)

Plugin de **mezcla y ecualización automática de percusión dominicana** — ORYMY And YMY Records.

Se inserta en **cada canal por separado** (güira, tambora, conga, bongó, timbal, percusión general o bus de percusión).

## Qué hace
- **Analiza** la señal en tiempo real: nivel RMS, brillo (centroide espectral) y espectro de 64 bandas.
- **Auto Mix**: ajusta la ganancia de entrada al nivel objetivo del preset y corrige el brillo hacia el brillo objetivo (límites ±18 dB y ±6 dB).
- **APRENDER**: analiza 8 s de percusión y fija la corrección. **CONTINUO**: vuelve a analizar siempre.
- **Cadena**: HPF, low shelf, mid paramétrico, presencia, aire, calor (saturación), transientes, compresor enlazado estéreo, ancho M/S, mezcla dry/wet, salida y limitador.
- **448 presets** = 7 instrumentos × 8 estilos × 8 caracteres:
  - Instrumentos: Güira, Tambora, Conga, Bongó, Timbal, Percusión, Bus Percusión.
  - Estilos: Merengue, Merengue Típico, Bachata Moderna, Bachata Tradicional, Bachata Urbana, Mambo, En Vivo, Estudio.
  - Caracteres: Natural, Brillante, Cálido, Seco, Pegao, Maco, Majao, Aireado.
- **SUGERIR PRESET**: carga el instrumento detectado (detección aproximada por brillo).

## Compilar en GitHub (sin instalar nada)
1. Crea un repositorio nuevo en GitHub y sube **todo el contenido** de esta carpeta (incluida `.github/`).
2. Ve a la pestaña **Actions** → *Build PERCUSSION ANALIZER* → espera ~10-15 min la primera vez.
3. Entra a la ejecución terminada → **Artifacts** → `PERCUSSION_ANALIZER_Windows`:
   - `PERCUSSION_ANALIZER_Setup.exe` (instalador con icono, instala en `C:\Program Files\Common Files\VST3`)
   - `PERCUSSION_ANALIZER_VST3_manual.zip`
4. Para publicar un release: crea un tag `v1.0.0` y el instalador se adjunta solo.

## En FL Studio
Instala, abre FL Studio → *Options > Manage plugins > Find installed plugins* → busca **PERCUSSION ANALIZER**.
Pon el plugin en el canal, toca la percusión, pulsa **APRENDER** y elige un preset.

## Notas
- Los valores de los presets son un punto de partida técnico: afínalos de oído con tu material.
- Licencia: JUCE es GPLv3 o licencia comercial. Si vas a vender el plugin cerrado, necesitas una licencia comercial de JUCE.
