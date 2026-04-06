#ifndef HUD_VALUE_HPP
#define HUD_VALUE_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"
#include "Util/TransformUtils.hpp"
#include <memory>
#include <string>

class HudValue {
public:
    HudValue(const std::string& fontPath, int fontSize, const Util::Color& color)
        : m_FontPath(fontPath), m_FontSize(fontSize), m_Color(color) {}

    // 更新並繪製數值
    void Draw(const std::string& value, glm::vec2 pos, float zIndex) {
        // 為了達到最佳效果，每次 Draw 都建立新的 Text (或只在數值改變時更新)
        m_Text = std::make_shared<Util::Text>(m_FontPath, m_FontSize, value, m_Color);

        Util::Transform t;
        t.translation = pos;

        // 使用 ConvertToUniformBufferData 進行繪製
        m_Text->Draw(Util::ConvertToUniformBufferData(t, m_Text->GetSize(), zIndex));
    }

private:
    std::shared_ptr<Util::Text> m_Text;
    std::string m_FontPath;
    int m_FontSize;
    Util::Color m_Color;
};

#endif