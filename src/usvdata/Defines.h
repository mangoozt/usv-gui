#ifndef USV_DEFINES_H
#define USV_DEFINES_H

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
     * \brief Тип зоны расхождения
     */
    enum class ZoneType {
        DangerNone = 0,
        Capture_7_to_12,
        Decision_5_to_7,
        Control_2_to_5,
        Close_less_2
    };

    /**
     * \brief Тип опасности судна
     */
    enum class DangerType {
        NotDangerous = 0,
        PotentiallyDangerous,
        Dangerous
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
    
}
#endif // USV_DEFINES_H
