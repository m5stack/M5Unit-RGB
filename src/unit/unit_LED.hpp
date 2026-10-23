/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_LED.hpp
  @brief LED base unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_LED_HPP
#define M5_UNIT_RGB_UNIT_LED_HPP

#include "unit_LED_data.hpp"
#include <M5UnitComponent.hpp>
#include <vector>

namespace m5 {
namespace unit {

namespace rgb {

/*!
  @enum Order
  @brief Color order
  @details Each name lists the bytes in the order they are sent, e.g. grb24 sends G, R, B
 */
enum class Order : int8_t {
    unknown = -1,  //!< Not set (begin() fails)
    // 24 bits (no white channel)
    rgb24,
    rbg24,
    grb24,
    gbr24,
    brg24,
    bgr24,
    // 32 bits (with white channel)
    hasWhite,  //!< First 32-bit order; orders >= hasWhite carry a white channel
    //
    wrgb32 = hasWhite,  //!< Same value as hasWhite
    wrbg32,
    wgrb32,
    wgbr32,
    wbrg32,
    wbgr32,
    //
    rwgb32,
    rwbg32,
    rgwb32,
    rgbw32,
    rbwg32,
    rbgw32,
    //
    gwrb32,
    gwbr32,
    grwb32,
    grbw32,
    gbwr32,
    gbrw32,
    //
    bwrg32,
    bwgr32,
    brwg32,
    brgw32,
    bgwr32,
    bgrw32,
};

/*!
  @brief Offset of Red
  @param f Color order
  @return Byte offset of Red in the packed pixel (-1 if not present)
 */
int8_t offsetR(const Order f);
/*!
  @brief Offset of Green
  @param f Color order
  @return Byte offset of Green in the packed pixel (-1 if not present)
 */
int8_t offsetG(const Order f);
/*!
  @brief Offset of Blue
  @param f Color order
  @return Byte offset of Blue in the packed pixel (-1 if not present)
 */
int8_t offsetB(const Order f);
/*!
  @brief Offset of White
  @param f Color order
  @return Byte offset of White in the packed pixel (-1 if not present)
 */
int8_t offsetW(const Order f);
}  // namespace rgb

/*!
  @class m5::unit::UnitLED
  @brief LED base unit
*/
class UnitLED : public Component {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitLED, 0x00);

    ///@cond
    class PixelProxy {
    public:
        PixelProxy(UnitLED& u, const uint32_t i) : _u(u), _i(i), _c{u.color(i)}
        {
        }

        inline operator rgb::Color() const
        {
            return _c;
        }

        PixelProxy& operator=(const rgb::Color& c)
        {
            _c = c;
            _u.setPixelColor(_i, _c);
            return *this;
        }
        PixelProxy& operator=(const PixelProxy& o)
        {
            _c = static_cast<rgb::Color>(o);
            _u.setPixelColor(_i, _c);
            return *this;
        }

    private:
        UnitLED& _u;
        uint32_t _i;
        rgb::Color _c;  // Color at the time of Proxy creation
    };
    ///@endcond

public:
    using container_type      = std::vector<uint8_t>;              //!< pixel container
    using item_container_type = std::vector<gpio::m5_rmt_item_t>;  //!< item container

    /*!
      @struct config_t
      @brief Settings for begin
     */
    struct config_t {
        uint8_t brightness{10};  //!< Initial brightness
        //! @brief RMT memory blocks (0 = use chip default). Ignored via PbHub
        uint8_t mem_blocks{0};
        //! @brief Use DMA for RMT (RMT v2 only, e.g. ESP32-S3/C6). Ignored via PbHub
        bool with_dma{false};
    };

    ///@name Configuration for begin
    ///@{
    /*!
      @brief Gets the configuration
      @return config_t
     */
    inline config_t config() const
    {
        return _cfg;
    }
    /*!
      @brief Set the configuration
      @param cfg config_t
     */
    inline void config(const config_t& cfg)
    {
        _cfg = cfg;
    }
    ///@}

    //! @brief Destructor
    virtual ~UnitLED()
    {
    }

    /*!
      @brief Connect an LED unit as a daisy-chained child
      @param c Child LED unit
      @param channel Channel number
      @return True if successful
      @note Shadows Component::add so only UnitLED subtypes are accepted as children of an LED daisy chain.
     */
    bool add(UnitLED& c, const int16_t channel);

    /*!
      @brief Begin the unit
      @return True if successful
     */
    virtual bool begin() override;

    ///@name Properties
    ///@{
    /*!
      @brief Number of the pixels
      @return Number of pixels
     */
    inline uint16_t numPixels() const
    {
        return _numPixels;
    }
    /*!
      @brief Brightness
      @return Current brightness (255 = 100%)
    */
    inline uint8_t brightness() const
    {
        return _brightness;
    }

    /*!
      @brief Logical width in pixels
      @return Width
     */
    inline virtual uint16_t width() const
    {
        return numPixels();
    }
    /*!
      @brief Logical height in pixels
      @return Height
     */
    inline virtual uint16_t height() const
    {
        return 1;
    }

    /*!
      @brief Has white data?
      @return True if the color order includes a white channel
     */
    inline bool hasWhite() const
    {
        return (_order >= rgb::Order::hasWhite);
    }
    /*!
      @brief Color order
      @return rgb::Order
     */
    inline rgb::Order order() const
    {
        return _order;
    }
    /*!
      @brief Offset of red
      @return Byte offset of Red (-1 if not present)
     */
    inline int8_t offsetR() const
    {
        return _offsetR;
    }
    /*!
      @brief Offset of green
      @return Byte offset of Green (-1 if not present)
     */
    inline int8_t offsetG() const
    {
        return _offsetG;
    }
    /*!
      @brief Offset of blue
      @return Byte offset of Blue (-1 if not present)
     */
    inline int8_t offsetB() const
    {
        return _offsetB;
    }
    /*!
      @brief Offset of white
      @return Byte offset of White (-1 if not present)
     */
    inline int8_t offsetW() const
    {
        return _offsetW;
    }

    /*!
      @brief pixels container
      @return Reference to the internal packed pixel buffer
     */
    const container_type& pixels() const
    {
        return _pixels;
    }
    /*!
      @brief Get the Color
      @param n Position of the pixels
      @return Color if exists
      @note Affected by rotation
    */
    inline rgb::Color color(const uint16_t n) const
    {
        return (n < _numPixels) ? get_pixel_color(index_of(n)) : rgb::Color();
    }
    /*!
      @brief Get the Color
      @param n Position of the pixels
      @return Color if exists
      @note Not affected by rotation
    */
    inline rgb::Color absoluteColor(const uint16_t n) const
    {
        return (n < _numPixels) ? get_pixel_color(n) : rgb::Color();
    }
    ///@}

    ///@name Apply
    ///@{
    /*!
      @brief Write pixel color to LED
      @return True if successful
     */
    bool writePixels();
    /*!
      @brief Write pixel color to LED (alias)
      @return True if successful
     */
    inline bool show()
    {
        return writePixels();
    }
    ///@}

    ///@note Calling writePixels() or show() is reflected in LEDs
    ///@name Modifiers
    ///@{
    /*!
      @brief Set the brightness
      @param b brightness (255 = 100%)
      @note Behaves similar to FastLED
      @note Internal pixel colors are not changed
      @note The brightness applied on writePixels()
     */
    inline void setBrightness(const uint8_t b)
    {
        _brightness = b;
    }
    /*!
      @brief Set pixel color (RGB)
      @param n Position of the pixels
      @param r Red
      @param g Green
      @param b Blue
      @return True if successful
      @note If white is supported, white value does not change
      @note Affected by rotation
     */
    bool setPixelColor(const uint16_t n, const uint8_t r, const uint8_t g, const uint8_t b);
    /*!
      @brief Set pixel color (RGBW)
      @param n Position of the pixels
      @param r Red
      @param g Green
      @param b Blue
      @param w White
      @return True if successful
      @note Affected by rotation
      @warning If white is not supported, white is ignored
     */
    inline bool setPixelColor(const uint16_t n, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w)
    {
        return setPixelColor(n, rgb::Color(r, g, b, w));
    }
    /*!
      @brief Set pixel color
      @param n Position of the pixels
      @param c WRGB 32bits color
      @return True if successful
      @note Affected by rotation
      @warning If white is not supported, white is ignored
     */
    inline bool setPixelColor(const uint16_t n, const rgb::wrgb32_color_t c)
    {
        return setPixelColor(n, rgb::Color(c));
    }
    /*!
      @brief Set pixel color
      @param n Position of the pixels
      @param c rgb::Color
      @return True if successful
      @note Affected by rotation
      @warning If white is not supported, white is ignored
     */
    inline bool setPixelColor(const uint16_t n, const rgb::Color& c)
    {
        if (n >= _numPixels) {
            return false;
        }
        return set_pixel_color(index_of(n), c);
    }
    /*!
      @brief Fill the specified range with the specified color
      @param c rgb::Color
      @param first First position of the pixels
      @param count Number of pixels to fill (To the end if zero)
      @return True if successful
      @note Affected by rotation
      @warning If white is not supported, white is ignored
     */
    bool fill(const rgb::Color& c, const uint16_t first = 0, const uint16_t count = 0);
    /*!
      @brief Clear pixels
      @note All pixel color to WRGB 0,0,0,0
     */
    inline void clear(void)
    {
        fill(rgb::Color());
    }
    /*!
      @brief Fill the specified range to rainbow
      @param first First position of the pixels
      @param count Number of pixels to fill (To the end if zero)
      @param firstHue The first hue value
      @return True if successful
      @note Affected by rotation
     */
    bool fillRainbow(const uint16_t first = 0, const uint16_t count = 0, const uint8_t firstHue = 0);
    /*!
      @brief Fill the specified range to gradation pixels
      @param firstClr First Color
      @param lastClr Last Color
      @param first First position of the pixels
      @param count Number of pixels to fill (To the end if zero)
      @return True if successful
      @note Affected by rotation
     */
    bool fillGradation(const rgb::Color& firstClr, const rgb::Color& lastClr, const uint16_t first = 0,
                       const uint16_t count = 0);
    /*!
      @brief Fill the specified range to random color pixels
      @param first First position of the pixels
      @param count Number of pixels to fill (To the end if zero)
      @return True if successful
      @note Affected by rotation
     */
    bool fillRandom(const uint16_t first = 0, const uint16_t count = 0);
    /*!
      @brief Shift pixels left
      @param count Number of shifts
      @note The empty space will be WRGB 0,0,0,0
      @note Affected by rotation
     */
    inline void shiftLeft(const uint16_t count = 1)
    {
        shift_left(count, false);
    }
    /*!
      @brief Shift pixels right
      @param count Number of shifts
      @note The empty space will be WRGB 0,0,0,0
      @note Affected by rotation
     */
    inline void shiftRight(const uint16_t count = 1)
    {
        shift_right(count, false);
    }
    /*!
      @brief Rotate pixels left
      @param count Number of rotates
      @note Affected by rotation
     */
    inline void rotateLeft(const uint16_t count = 1)
    {
        shift_left(count, true);
    }
    /*!
      @brief Rotate pixels right
      @param count Number of rotates
      @note Affected by rotation
     */
    inline void rotateRight(const uint16_t count = 1)
    {
        shift_right(count, true);
    }
    ///@}

    ///@note Calling writePixels() or show() is reflected in LEDs
    ///@note 1 rot angle varies from unit to unit (e.g. Puzzle 90 deg Hex 60 deg)
    ///@note Rotation direction is clockwise
    ///@name Rotation
    ///@{
    /*!
      @brief Get the max rotation
      @return Number of discrete rotation steps
     */
    inline virtual uint8_t maxRotation() const
    {
        return 2;  // 0,180 deg
    }
    /*!
      @brief Get the rotation angle per 1
      @return Angle per rotation step (degree)
     */
    inline float rotationDegree() const
    {
        return 360.f / maxRotation();
    }
    /*!
      @brief Get the rotation
      @return Current rotation step
     */
    inline int8_t rotation() const
    {
        return _rot;
    }
    /*!
      @brief Set the rotation
      @param rot Rotation step
     */
    void setRotation(const int8_t rot);
    ///@}

    ///@note Affected by rotation
    ///@name Pixels element access
    ///@{
    /*!
      @brief rvalue
      @param i Position of the pixels
      @return Color at position i
     */
    rgb::Color operator[](const uint16_t i) const
    {
        return color(i);
    }
    /*!
      @brief lvalue
      @param i Position of the pixels
      @return Proxy that writes back on assignment
     */
    PixelProxy operator[](const uint16_t i)
    {
        return PixelProxy(*this, i);
    }
    ///@}

    ///@cond
    // Class that abstracts the interaction between classes and adapters
    class Interface {
    public:
        explicit Interface(UnitLED& u) : _unit{u}
        {
        }
        virtual ~Interface()
        {
        }
        virtual bool write(const uint8_t*, const uint32_t) = 0;
        // For RGB888
        virtual UnitLED::container_type rgb888_items()
        {
            return {};
        }
        virtual UnitLED::container_type rgb888_items_from_root()
        {
            return {};
        }

        // Write pixels directly via the protocol-specific method
        virtual bool writePixelsDirect()
        {
            return false;
        }

    protected:
        UnitLED& _unit;
    };
    ///@endcond

protected:
    inline virtual types::category_t unit_category() const override
    {
        return types::category_t::UnitLED;
    }
    inline virtual gpio::adapter_config_t rmt_config()
    {
        return gpio::adapter_config_t{};
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_one()
    {
        return gpio::m5_rmt_item_t{};
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_zero()
    {
        return gpio::m5_rmt_item_t{};
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_reset()
    {
        return gpio::m5_rmt_item_t{};
    }
    inline container_type& pixels()
    {
        return _pixels;
    }

    // Not affected by rotation
    rgb::Color get_pixel_color(const uint16_t n) const;
    bool set_pixel_color(const uint16_t n, const rgb::Color& c);

    inline uint8_t rotation_of(const int8_t r) const
    {
        return (r % maxRotation() + maxRotation()) % maxRotation();
    }

    inline virtual uint16_t index_of(const uint16_t n) const
    {
        // 0 or 180 deg
        return _rot ? _numPixels - (n % _numPixels) - 1 : n;
    }

    container_type rgb888_items();

    void shift_left(const uint16_t count, const bool wrap);
    void shift_right(const uint16_t count, const bool wrap);

protected:
    // Protected so only derived types with hardcoded Order (UnitRGB / UnitPuzzle / ...) can construct.
    explicit UnitLED(const uint16_t num_of_pixels, const rgb::Order order)
        : Component(0x00), _order{order}, _numPixels{num_of_pixels}
    {
        _offsetR = rgb::offsetR(_order);
        _offsetG = rgb::offsetG(_order);
        _offsetB = rgb::offsetB(_order);
        _offsetW = rgb::offsetW(_order);
        _pixels.resize(_numPixels * (3 + hasWhite()));
    }

private:
    rgb::Order _order{rgb::Order::unknown};
    uint8_t _brightness{10};
    int8_t _rot{};
    int8_t _offsetR{-1}, _offsetG{-1}, _offsetB{-1}, _offsetW{-1};
    uint16_t _numPixels{};
    bool _ready{false};
    container_type _pixels{};  // Order by rgb::Order
    std::unique_ptr<Interface> _interface{};
    config_t _cfg{};

    friend class InterfaceGPIO;
    friend class InterfacePbHub;
};

}  // namespace unit
}  // namespace m5
#endif
