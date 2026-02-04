window.METROVIAN = {
  state: 0,
};

document.addEventListener("DOMContentLoaded", () => {
  const STATE_UI = {
    1: { label: "SETUP", color: "#3b82f6" },
    2: { label: "SYNTHESIS", color: "#2fbdd6" },
    3: { label: "READY", color: "#22f7b6" },
    4: { label: "PLAY", color: "#22f7b6" },
    502: { label: "ERROR", color: "#ef4444" },
  };

  const stateText = document.getElementById("stateText");
  const stateLine = document.getElementById("stateLine");

  async function updateState() {
    const prev = METROVIAN.state;
    try {
      const raw = await asyncAPI({ action: "read", state: 0 });
      const state = raw.trim();
      METROVIAN.state = state;
    } catch (err) {
      METROVIAN.state = 502;
    }

    if (METROVIAN.state != prev) {
      const info = STATE_UI[METROVIAN.state];
      stateText.textContent = info.label;
      stateLine.style.setProperty("--state-color", info.color);
      window.dispatchEvent(
        new CustomEvent("changeState", {
          detail: METROVIAN.state,
        })
      );

      if (METROVIAN.state != 2) {
        stateLine.style.setProperty("--progress", 100);
      }
    }

    if (METROVIAN.state == 2) {
      const now = await asyncAPI({ action: "read", progress: 0 });
      const max = await asyncAPI({ action: "read", notes: 0 });
      stateLine.style.setProperty(
        "--progress",
        (parseInt(now) * 100.0) / parseInt(max)
      );
    }

    return;
  }

  updateState();
  setInterval(updateState, 200);
  return;
});
