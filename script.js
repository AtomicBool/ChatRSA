const canvas = document.getElementById('mosaic-canvas');
const ctx = canvas.getContext('2d', { alpha: true });
const wrapper = document.getElementById('mosaic-wrapper');

const text = "ChatRSA";
const fontSize = 80;
const fontWeight = '900';
const fontFace = '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif';

let pixelSize = 20;
let targetPixelSize = 20;
const minPixelSize = 1;
const maxPixelSize = 20;

// Offscreen canvas for drawing text clearly
const offscreenCanvas = document.createElement('canvas');
const offscreenCtx = offscreenCanvas.getContext('2d');

function init() {
    const tempCtx = offscreenCanvas.getContext('2d');
    tempCtx.font = `${fontWeight} ${fontSize}px ${fontFace}`;
    const metrics = tempCtx.measureText(text);
    const textWidth = metrics.width;
    const textHeight = fontSize * 1.5; // Extra height for descenders

    canvas.width = textWidth + 60;
    canvas.height = textHeight + 20;
    offscreenCanvas.width = canvas.width;
    offscreenCanvas.height = canvas.height;

    // Reset font after width/height change
    offscreenCtx.fillStyle = 'white';
    offscreenCtx.font = `${fontWeight} ${fontSize}px ${fontFace}`;
    offscreenCtx.textBaseline = 'middle';
    offscreenCtx.textAlign = 'center';
    offscreenCtx.fillText(text, offscreenCanvas.width / 2, offscreenCanvas.height / 2);

    if (!window.animationStarted) {
        window.animationStarted = true;
        animate();
    }
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Pixelation technique: draw offscreen canvas scaled down, then scale up
    const w = canvas.width;
    const h = canvas.height;
    
    // If pixelSize is 1, just draw the original text for crispness
    if (pixelSize <= 1.1) {
        ctx.drawImage(offscreenCanvas, 0, 0);
        return;
    }

    // Temporary small canvas for pixelation
    const tempCanvas = document.createElement('canvas');
    const tempCtx = tempCanvas.getContext('2d');
    tempCanvas.width = Math.ceil(w / pixelSize);
    tempCanvas.height = Math.ceil(h / pixelSize);

    // Disable smoothing
    ctx.imageSmoothingEnabled = false;
    tempCtx.imageSmoothingEnabled = false;

    // Scale down
    tempCtx.drawImage(offscreenCanvas, 0, 0, w, h, 0, 0, tempCanvas.width, tempCanvas.height);
    
    // Scale up
    ctx.drawImage(tempCanvas, 0, 0, tempCanvas.width, tempCanvas.height, 0, 0, w, h);
}

function animate() {
    // Smoothly interpolate pixelSize
    const ease = 0.05;
    pixelSize += (targetPixelSize - pixelSize) * ease;

    draw();
    requestAnimationFrame(animate);
}

wrapper.addEventListener('mouseenter', () => {
    targetPixelSize = minPixelSize;
});

wrapper.addEventListener('mouseleave', () => {
    targetPixelSize = maxPixelSize;
});

// Handle window resizing or initial load fonts
window.addEventListener('load', init);

// Responsive canvas
function resize() {
    // Simple refresh on resize if needed
    // In this minimalist case, we keep it simple
}
window.addEventListener('resize', resize);
