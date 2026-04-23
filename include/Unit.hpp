#ifndef UNIT_HPP
#define UNIT_HPP

#include <vector>
#include <glm/glm.hpp>
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Util/TransformUtils.hpp"

class Unit : public Util::GameObject {
public:
    Unit(const std::vector<glm::vec2>& path, float speed, float hp)
        : m_Path(path), m_Speed(speed), m_HP(hp), m_MaxHP(hp) {
        if (!m_Path.empty()) m_Transform.translation = m_Path[0];
        m_ZIndex = 15.0f; // 確保單位有基本的 ZIndex
    }

    virtual ~Unit() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    float GetHP() const { return m_HP; }

protected:
    // 將預設值從 30.0f 改為更小的數值，例如 15.0f 或更低
    // 如果想要更貼近，可以嘗試 10.0f 或 5.0f
    void DrawHealthBar(float yOffset = 15.0f) {
        if (m_HP <= 0) return;

        // 確保比率正確
        float safeMax = (m_MaxHP <= 0) ? m_HP : m_MaxHP;
        float ratio = m_HP / safeMax;
        if (ratio > 1.0f) ratio = 1.0f;

        // --- 以下完全維持你要求的原始設定，不作任何變更 ---
        float bgScaleX = 0.7f;
        float bgScaleY = 0.7f;

        float fgScaleX = 1.2f;
        float fgScaleY = 1.2f;

        static auto bgImg = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/10146.png");
        static auto fgImg = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/10143.png");

        if (!bgImg || !fgImg) return;

        // 1. 繪製紅色底條
        Util::Transform bgTransform;
        bgTransform.translation = m_Transform.translation + glm::vec2(0, yOffset);
        bgTransform.scale = {bgScaleX, bgScaleY};

        auto bgData = Util::ConvertToUniformBufferData(bgTransform, bgImg->GetSize(), m_ZIndex + 0.1f);
        bgImg->Draw(bgData);

        // 2. 繪製綠色血條
        Util::Transform fgTransform;
        float currentFgScaleX = fgScaleX * ratio;

        float bgRealWidth = bgImg->GetSize().x * bgScaleX;
        float fgRealWidth = fgImg->GetSize().x * currentFgScaleX;

        float offsetX = (bgRealWidth - fgRealWidth) / 2.0f;

        fgTransform.translation = bgTransform.translation - glm::vec2(offsetX, 0);
        fgTransform.scale = {currentFgScaleX, fgScaleY};

        auto fgData = Util::ConvertToUniformBufferData(fgTransform, fgImg->GetSize(), m_ZIndex + 0.2f);
        fgImg->Draw(fgData);
    }

    void MoveTowardsNextNode() {
        if (m_CurrentNodeIdx >= m_Path.size()) return;
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        glm::vec2 targetPos = m_Path[m_CurrentNodeIdx];
        glm::vec2 dir = targetPos - m_Transform.translation;
        float dist = glm::length(dir);
        if (dist > 1.0f) {
            m_Transform.translation += glm::normalize(dir) * m_Speed * dt;
        } else { m_CurrentNodeIdx++; }
    }

    std::vector<glm::vec2> m_Path;
    size_t m_CurrentNodeIdx = 1;
    float m_Speed = 0.0f;
    float m_HP = 0.0f;
    float m_MaxHP = 0.0f;
};

#endif