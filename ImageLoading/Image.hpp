#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <windows.h>
#include <vector>

class ImageManager; // Forward declaration

// ---------------------------------------------------------
// ImageItem: represents one loaded bitmap + its draw params
// ---------------------------------------------------------
class ImageItem {
public:
    // Factory: load a BMP from disk and build an ImageItem.
    // If loading fails, returns an "empty" ImageItem (IsValid() == false).
    static ImageItem Create(LPCTSTR bmpPath,
                            int xOffset,
                            int yOffset,
                            int targetWidth,
                            int targetHeight)
    {
        ImageItem item; // starts empty

        HBITMAP hbmp = (HBITMAP)LoadImage(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (!hbmp) {
            return item; // invalid
        }

        BITMAP bm{};
        GetObject(hbmp, sizeof(bm), &bm);

        item.hBmp        = hbmp;
        item.widthBmp    = bm.bmWidth;
        item.heightBmp   = bm.bmHeight;
        item.widthImage  = targetWidth;
        item.heightImage = targetHeight;
        item.xOffset     = xOffset;
        item.yOffset     = yOffset;
        return item;
    }

    // Non-copyable (avoid double DeleteObject)

    // Moveable
   

    // Public helper to check validity
    bool IsValid() const { return hBmp != nullptr; }


private:
    // Private constructor => only factory & friends build real instances
    ImageItem() = default;

    void Destroy() {
        if (hBmp) {
            DeleteObject(hBmp);
            hBmp = nullptr;
        }
    }
    void MoveFrom(ImageItem &other) {
        hBmp        = other.hBmp;
        widthBmp    = other.widthBmp;
        heightBmp   = other.heightBmp;
        widthImage  = other.widthImage;
        heightImage = other.heightImage;
        xOffset     = other.xOffset;
        yOffset     = other.yOffset;

        other.hBmp = nullptr; // prevent double free
    }

    // --- Data members (private) ---
    HBITMAP hBmp        = nullptr;
    int     widthBmp    = 0;
    int     heightBmp   = 0;
    int     widthImage  = 0;
    int     heightImage = 0;
    int     xOffset     = 0;
    int     yOffset     = 0;

    friend class ImageManager;
};


std::vector<ImageItem> g_images;  // Stores every loaded copy

void AddImage(const ImageItem &img) {
    if (img.IsValid()) {
        g_images.push_back(img);
    }
}



// ---------------------------------------------------------
// ImageManager: owns multiple ImageItems
// ---------------------------------------------------------
class ImageManager {
public:
    // Configuration constants
    static constexpr int TARGET_W = 60;
    static constexpr int TARGET_H = 40;
    static constexpr int MARGIN   = 8;

    ImageManager() = default;
    ~ImageManager() { Clear(); }

    // Add a bitmap; returns true on success.
    bool AddBitmap(LPCTSTR path,
                   int xOffset = 0,
                   int yOffset = 0,
                   int targetW = TARGET_W,
                   int targetH = TARGET_H)
    {
        ImageItem item = ImageItem::Create(path, xOffset, yOffset, targetW, targetH);
        if (!item.IsValid())
            return false;
        items.push_back(std::move(item));
        return true;
    }

    // Draw everything. Optionally vertical scroll offset passed in.
    void DrawAllImages(HDC hdc, int scrollOffsetY = 0) const
    {
        for (const auto &img : g_images) {
            if (!img.IsValid()) continue;

            HDC srcDC = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = (HBITMAP)SelectObject(srcDC, img.hBmp);

            SetStretchBltMode(hdc, HALFTONE);
            SetBrushOrgEx(hdc, 0, 0, NULL);

            int drawX = img.xOffset * 25;
            int drawY = img.yOffset * 25;

            StretchBlt(hdc, drawX + 5, drawY + 5, img.widthImage * 25 - 5, img.heightImage * 25 - 5, srcDC, 0, img.heightBmp - 1,      // ySrc: start at *bottom* scanline
           img.widthBmp, -img.heightBmp, SRCCOPY);

            HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            SelectObject(hdc, oldBrush);

            SelectObject(srcDC, oldBmp);
            DeleteDC(srcDC);
        }
    }

    // Remove all
    void Clear() {
        // Individual ImageItem destructors handle deletion.
        items.clear();
    }

    size_t Count() const { return items.size(); }

    // Access (const) if needed
    const ImageItem& Get(size_t idx) const { return items[idx]; }
    ImageItem&       Get(size_t idx)       { return items[idx]; }

    void SetBaseYOffset(int y) { baseYOffset = y; }

private:
    std::vector<ImageItem> items;
    int baseYOffset = 24; // for stacked layout convenience
};

#endif // IMAGE_HPP
