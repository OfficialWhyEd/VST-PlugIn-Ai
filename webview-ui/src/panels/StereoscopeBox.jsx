import { useState, useMemo } from 'react'
import { motion } from 'framer-motion'
import BoxWrapper from './shared/BoxWrapper'

const POSA = [0.16, 1, 0.3, 1]

// Dai campioni veri si ricava dove sta l'energia nel campo stereo. Prima
// qui c'erano quattro barre calcolate dai soli livelli L e R, e la
// correlazione era "0,62 + (L+R) × 0,15": un numero sempre positivo, quindi
// incapace di segnalare proprio il problema per cui si guarda un imager.
function analizzaCampo(points) {
  if (!points || points.length < 4) return null

  const SETTORI = 15          // dispari: uno sta esattamente al centro
  const istogramma = new Array(SETTORI).fill(0)
  let sommaSide = 0, sommaMid = 0, n = 0

  for (let i = 0; i + 1 < points.length; i += 2) {
    const side = points[i]
    const mid = points[i + 1]
    sommaSide += side * side
    sommaMid += mid * mid
    n++

    const ampiezza = Math.hypot(side, mid)
    if (ampiezza < 0.002) continue      // silenzio: non dice nulla sulla posizione

    // L'angolo rispetto all'asse mid dice da che parte sta il suono.
    // −1 tutto a sinistra, 0 centro, +1 tutto a destra.
    const posizione = Math.max(-1, Math.min(1, Math.atan2(side, Math.abs(mid)) / (Math.PI / 2)))
    const settore = Math.min(SETTORI - 1, Math.floor((posizione + 1) / 2 * SETTORI))
    istogramma[settore] += ampiezza
  }

  if (!n) return null
  const picco = Math.max(...istogramma)
  if (picco <= 0) return null

  const side = Math.sqrt(sommaSide / n)
  const mid = Math.sqrt(sommaMid / n)

  return {
    istogramma: istogramma.map(v => v / picco),
    quotaSide: side + mid > 0 ? side / (side + mid) : 0,
    larghezza: mid > 1e-6 ? Math.min(2, side / mid) : (side > 0 ? 2 : 0),
  }
}

export default function StereoscopeBox({ meterL = 0, meterR = 0, correlation = 0, scopePoints = [], onDawCmd, ...rest }) {
  const [midSide, setMidSide] = useState(false)
  const campo = useMemo(() => analizzaCampo(scopePoints), [scopePoints])

  const bilanciamento = Math.round((meterR - meterL) * 100)   // negativo = più a sinistra
  const larghezzaPct = campo ? Math.round(campo.larghezza * 100) : 0
  const sidePct = campo ? Math.round(campo.quotaSide * 100) : 0

  const monoOk = correlation >= 0.3
  const tinta = !campo ? '#6f6f6f' : monoOk ? '#DC143C' : '#FFB000'

  return (
    <BoxWrapper label="Stereo Field" color="#DC143C" icon="swap_horiz" {...rest}>
      {/* Il campo, disegnato. Ogni colonna e' una posizione fra sinistra e
          destra, la sua altezza e' quanta energia ci sta. */}
      <div className="rounded-[14px] p-[3px] bg-white/[0.04] ring-1 ring-white/[0.06] mb-2.5">
        <div className="relative rounded-[11px] bg-[#0a0a0a] h-[74px] overflow-hidden">
          {/* Riferimenti: centro pieno, laterali tratteggiati */}
          <div className="absolute inset-y-0 left-1/2 w-px bg-white/[0.10]" />
          <div className="absolute inset-y-0 left-[16.6%] w-px bg-white/[0.04]" />
          <div className="absolute inset-y-0 left-[83.3%] w-px bg-white/[0.04]" />

          {campo ? (
            <div className="absolute inset-x-2 bottom-2 top-3 flex items-end gap-[3px]">
              {campo.istogramma.map((v, i) => {
                const centro = Math.abs(i - (campo.istogramma.length - 1) / 2) / ((campo.istogramma.length - 1) / 2)
                return (
                  <motion.div
                    key={i}
                    className="flex-1 rounded-[2px] origin-bottom"
                    style={{
                      // Al centro il rosso del marchio, ai lati vira all'ambra:
                      // la posizione si legge dal colore prima che dall'altezza.
                      background: `linear-gradient(to top, ${centro > 0.55 ? '#FFB000' : '#DC143C'}, ${centro > 0.55 ? '#FFB00055' : '#DC143C55'})`,
                    }}
                    initial={false}
                    animate={{ height: `${Math.max(2, v * 100)}%` }}
                    transition={{ duration: 0.22, ease: POSA }}
                  />
                )
              })}
            </div>
          ) : (
            <div className="absolute inset-0 flex items-center justify-center text-[9px] font-mono text-[#5a5a5a]">
              nessun segnale
            </div>
          )}

          <div className="absolute inset-x-0 bottom-0.5 flex justify-between px-2.5 text-[7px] font-mono text-[#5a5a5a] pointer-events-none">
            <span>L</span><span>C</span><span>R</span>
          </div>
        </div>
      </div>

      {/* Tre numeri, in ordine di quanto contano davvero */}
      <div className="grid grid-cols-3 gap-2 mb-2.5">
        {[
          {
            etichetta: 'larghezza',
            valore: campo ? `${larghezzaPct}%` : '—',
            colore: '#00FFaa',
            nota: larghezzaPct > 140 ? 'molto larga' : larghezzaPct < 25 ? 'quasi mono' : null,
          },
          {
            etichetta: 'lato',
            valore: campo ? `${sidePct}%` : '—',
            colore: '#FFB000',
            nota: null,
          },
          {
            etichetta: 'bilanciamento',
            valore: campo ? (bilanciamento === 0 ? 'centro' : `${Math.abs(bilanciamento)}% ${bilanciamento < 0 ? 'L' : 'R'}`) : '—',
            colore: '#DC143C',
            nota: Math.abs(bilanciamento) > 20 ? 'sbilanciato' : null,
          },
        ].map(m => (
          <div key={m.etichetta}>
            <div className="font-mono font-bold text-[15px] leading-none tabular-nums" style={{ color: m.colore }}>
              {m.valore}
            </div>
            <div className="text-[8px] uppercase tracking-[0.12em] text-[#7a7a7a] mt-1">{m.etichetta}</div>
            {m.nota && <div className="text-[8px] text-[#9a9a9a] mt-0.5">{m.nota}</div>}
          </div>
        ))}
      </div>

      {/* L'avviso compare solo quando serve, e dice cosa succede in pratica */}
      {campo && !monoOk && (
        <motion.div
          initial={{ opacity: 0, y: -4 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.4, ease: POSA }}
          className="mb-2.5 rounded-lg px-2.5 py-1.5 text-[10px] leading-snug"
          style={{ backgroundColor: `${tinta}14`, color: tinta }}
        >
          In mono parte del suono si cancella: correlazione {correlation.toFixed(2)}.
        </motion.div>
      )}

      <div className="flex gap-1">
        {[
          { testo: 'Mid/Side', attivo: midSide, tinta: '#DC143C',
            click: () => { setMidSide(!midSide); onDawCmd('midSide', { enabled: !midSide }) } },
          { testo: 'Stringi', attivo: false, tinta: '#00FFaa', click: () => onDawCmd('narrow') },
          { testo: 'Allarga', attivo: false, tinta: '#FFB000', click: () => onDawCmd('widen') },
        ].map(b => (
          <motion.button
            key={b.testo}
            whileTap={{ scale: 0.96 }}
            onClick={b.click}
            className="flex-1 rounded-full py-1 text-[9px] font-bold tracking-tight transition-all duration-300"
            style={b.attivo
              ? { backgroundColor: b.tinta, color: '#0a0a0a' }
              : { color: '#9a9a9a', boxShadow: 'inset 0 0 0 1px rgba(255,255,255,0.10)' }}
          >
            {b.testo}
          </motion.button>
        ))}
      </div>
    </BoxWrapper>
  )
}
