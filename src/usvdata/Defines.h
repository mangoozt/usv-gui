#ifndef USV_DEFINES_H
#define USV_DEFINES_H
#define _USE_MATH_DEFINES
namespace USV {
    /**
     * \brief Категория судна в контексте правила 18 МППСС-72
     */
    enum ShipCategory {
        SCPropelled = 0,     //! 0 - судно, оборудованное механическим двигателем;
        SCSailing = 1,       //! 1 - парусное судно;
        SCFishing = 2,       //! 2 - судно, занятое ловом рыбы;
        SCSteerImpaired = 3, //! 3 - судно, ограниченное в возможности маневрировать;
        SCUnmanned = 4,      //! 4 - судно, лишенное возможности управления;
        SCDraft = 5,         //! 5 - судно, ограниченное своей осадкой;
        SCFollowingTSS = 6,  //! 6 - судно, оборудованное механическим двигателем, следующее по СРД
    };

    /**
     * \brief Тип опасности судна
     */
    enum class DangerType {
        NotDangerous = 0,
        PotentiallyDangerous,
        Dangerous,
        Undefined
    };

    struct SafeZone {
        int safety_zone_type;
        double radius;
        double start_angle;
        double end_angle;
        double length;
        double width;
        double lengthOffset;
    };

    /**
 * \brief Тип сценари¤ расхождения согласно ТЗ
 */
    enum class ScenarioType {
        Sc0_None = 0,
        Sc1_FaceToFace,
        Sc2_Overtaken,
        Sc3_Overtake,
        Sc4_GiveWay,
        Sc5_Save,
        Sc6_GiveWayPriority,
        Sc7_SavePriority,
        Sc8_CrossMove,
        Sc9_CrossIn,
        Sc10_VisionRestricted_Fwd,
        Sc11_VisionRestricted_Bwd
    };
}
#endif // USV_DEFINES_H
