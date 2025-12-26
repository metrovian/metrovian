document.addEventListener("DOMContentLoaded", () => {
  const uploadBtn = document.getElementById("uploadBtn");
  const saveBtn = document.getElementById("saveBtn");
  const rebootBtn = document.getElementById("rebootBtn");
  const shutdownBtn = document.getElementById("shutdownBtn");

  uploadBtn.addEventListener("click", async () => {
    const fileInput = document.createElement("input");
    fileInput.onchange = async (event) => {
      const file = event.target.files[0];
      if (!file) {
        return;
      }

      try {
        const data = JSON.parse(await file.text());
        const res = await fetch("/api?action=upload", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(data),
        });

        if (res.ok == false) {
          throw new Error(`HTTP ${res.status}`);
        }
      } catch (err) {
        console.warn("Upload", err);
        return;
      }

      alert("OK!");
      console.log("Upload");
      return;
    };

    fileInput.type = "file";
    fileInput.accept = ".json";
    fileInput.click();
    return;
  });

  saveBtn.addEventListener("click", async () => {
    try {
      const res = await asyncAPI({ action: "read", waveform: 0 });
      const data = JSON.parse(res);
      const blob = new Blob([JSON.stringify(data, null, 2)], {
        type: "application/json",
      });

      const url = URL.createObjectURL(blob);
      const elem = document.createElement("a");
      elem.href = url;
      elem.download = "preset.json";
      elem.click();
      URL.revokeObjectURL(url);
    } catch (err) {
      console.warn("Save", err);
      return;
    }

    console.log("Save");
    return;
  });

  rebootBtn.addEventListener("click", async () => {
    try {
      await asyncAPI({ action: "write", reboot: 1 });
    } catch (err) {
      console.warn("Reboot", err);
      return;
    }

    console.log("Reboot");
    return;
  });

  shutdownBtn.addEventListener("click", async () => {
    try {
      await asyncAPI({ action: "write", reboot: 0 });
    } catch (err) {
      console.warn("Shutdown", err);
      return;
    }

    console.log("Shutdown");
    return;
  });

  return;
});
