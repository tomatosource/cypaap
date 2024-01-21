  now = new Date();
  year = now.getFullYear();
  startOfYear = new Date(year, 0, 1);
  daysInYear = (
    (year % 4 === 0 && year % 100 !== 0) || (year % 400 === 0)
  ) ? 366 : 365;
  yearMs = daysInYear * 24 * 60 * 60 * 1000;

  update = () => {
    now = new Date();
    deltaMs = now - startOfYear;
    progress = deltaMs / yearMs * 100;
    document.body.innerText = `${progress.toFixed(7)}%`;
  }
  setInterval(update, 15);
